// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <console.h>
#include "tests.h"

extern void INVALID_OP_CODE();						 // Excepcion de codigo invalido
static char consoleBuffer[MAX_CONSOLE_BUFFER] = {0}; // buffer de la consola
static int lastChar = 0;							 // ultima posicion del buffer

char historyBuffer[MAX_COMMANDS][MAX_COMMAND_LENGTH]; // historial del buffer
unsigned int historyIndex = 0;						  // indice del historial
unsigned int historyDim = 0;						  // dimension del historial

// funcion de hashe de nuestro comandos https://stackoverflow.com/questions/4014827/how-can-i-compare-strings-in-c-using-a-switch-statement
const unsigned long hash(char *str)
{
	unsigned int hash = 5381;
	int c;

	while ((c = *str++))
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}

// Limpia el buffer de la consola
void clearconsoleBuffer()
{
	for (int i = 0; i < lastChar; i++)
		consoleBuffer[i] = 0;
	lastChar = 0;
}

// Limpia la consola y resetea el cursor
void clearScreen()
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	restartCursor();
}

// Espera que el usuario presione la tecla key
void waitForKey(char key)
{
	while (1)
	{
		if (getchar() == key)
		{
			break;
		}
	}
	// and restart
	clearScreen();
	restartCursor();
}

// Imprime el tiempo actual
void printCurrentTime()
{
	int aux = bcdToDec(getTime(0x04));
	if (aux < 3)
	{
		aux += aux + 24 - 3;
	}
	else
	{
		aux -= 3;
	}
	printInt(aux);
	putchar(':');
	printInt(bcdToDec(getTime(0x02)));
	putchar(':');
	printInt(bcdToDec(getTime(0x00)));
	putchar('\n');
}

// Comando help que imprime todos los comandos disponibles
void *commandHelp(int argc, char **argv)
{
	clearScreen();
	printColor("BIENVENIDO AL MENU HELP", 0xE9AD0C, 0);
	print("\nEL SISTEMA CUENTA CON LOS SIGUIENTES COMANDOS: <User commands>");
	print("\n------------------- ARQUI ---------------------");
	print("\n- HELP                       /* imprime help                    */");
	print("\n- TRON                       /* jugar tron                      */");
	print("\n- INFOREG                    /* imprime inforeg                 */");
	print("\n- MEMACCESS <0xMEMACCESS>    /* accede a la memoria             */");
	print("\n- BEEP                       /* ruidito                         */");
	print("\n- TIME                       /* hora                            */");
	print("\n- SIZE<+/->                  /* incrementa decrementa           */");
	printColor("\n- ANTHEM                     /* himno sovietico                 */", TERMINAL_BLUE, 0);
	printColor("\n- CLEAR                      /* clear                           */", TERMINAL_BLUE, 0);
	printColor("\n- PIANO                      /* piano                           */", TERMINAL_BLUE, 0);
	printColor("\n- LANGUAGE=<EN/ES>           /* cambia teclado                  */", TERMINAL_BLUE, 0);
	printColor("\n- INVOP                      /* invop                           */", RED, 0);
	printColor("\n- DIVCERO                    /* divcero                         */", RED, 0);
	print("\n------------------- SO ------------------------");
	print("\n- MEM                        /* imprime el estado de la memoria */");
	print("\n- PS                         /* imprime los procesos            */");
	print("\n- LOOP                       /* loopea infinitamente            */");
	print("\n- KILL <PID>                 /* mata el proceso con el pid dado */");
	print("\n- NICE <PID> <PRIORITY>      /* cambia la prioridad del proceso */");
	print("\n- BLOCK <PID>                /* bloquea el proceso              */");
	print("\n- CAT                        /* cat                             */");
	print("\n- WC                         /* wc                              */");
	print("\n- FILTER                     /* filtra vocales                  */");
	print("\n- PHYLO                      /* dilema de los filosofos         */");
	print("\n------------------- TESTS ---------------------");
	print("\n- TEST_MM                    /* test de memoria                 */");
	print("\n- TEST_PROC                  /* test de procesos                */");
	print("\n- TEST_PRIO                  /* test de round robin             */");
	print("\n- TEST_SYNC                  /* test de sincronizacion          */");
	print("\n- TEST_NO_SYNC               /* test de no sincronizado         */");
	print("\n");
	exit();
	return NULL;
}

// Comando clear que limpia la pantalla
void commandClear()
{
	clearScreen();
}

// Comando que imprime los registros
void commandInforeg()
{
	clearScreen();
	getRegisters();
	waitForKey(ESC);
}

// Comando que imprime el tiempo
void commandTime()
{
	printCurrentTime();
}

// Comando que ejecuta el juego tron
void commandTron()
{
	playTron();
	restartCursor();
	clearScreen();
}

// Comando que ejecuta la aplicacion de piano
void commandPiano()
{
	print("Usted se encuentra frente a un teclado especial.\n\n", 0);
	print("Presione teclas...\n\n", 0);
	appendstring("Para salir presione la 'SPACE' bar.\n\n");
	playPiano();
	commandClear();
}

// Comando que hace ruidito
void commandBeep()
{
	beep(1000, 10);
}

// Comando que imprime los 32 bytes siguientes a la direccion de memoria dada
void commandMemAccess(char *memdirHexa)
{
	if (strlen(memdirHexa) > 16 || !onlyHexChars(memdirHexa))
	{
		printColor("Error : ingrese una direccion en hexadecimal de 64 bits.\n", ORANGY);
		return;
	}
	clearScreen();
	uint64_t num = hexstringToInt(memdirHexa);
	// tal vez por la cantidad de memoria asignada para el proceso, o a la VM
	// del so o algo de paginacion, pero no se me permite desreferenciar direcciones
	// mayores al 0xFFFFFFFE0
	if (num <= 0xFFFFFFFE0)
	{
		memAccess(num);
		waitForKey(ESC);
	}
	else
	{
		printColor("Se esta accediendo a una direccion demasiado grande.", ORANGY);
		newline();
		appendstring("Presione ");
		printColor("'ESC'", 0xE9AD0C, 0);
		print(" para volver a la consola.\n", 0);
		waitForKey(ESC);
	}
}

// Comando que ejecuta excepcion de division por cero
void commandDivCero()
{
	clearScreen();
	int x = 1;
	int y = 0;
	int z = x / y;
	while (z)
		break;
}

// Comando que ejecuta himno de la URSS (nos llamamos SOviet es un chiste plz no kill)
void commandAnthem()
{
	beep(392, 375 / 50);
	beep(523, 750 / 50);
	beep(392, 463 / 50);
	beep(440, 187 / 50);
	beep(494, 750 / 50);
	beep(330, 375 / 50);
	beep(330, 375 / 50);
	beep(440, 750 / 50);
	beep(392, 463 / 50);
	beep(349, 187 / 50);
	beep(392, 750 / 50);
	beep(262, 463 / 50);
	beep(262, 187 / 50);
	beep(294, 750 / 50);
	beep(294, 463 / 50);
	beep(330, 187 / 50);
	beep(349, 750 / 50);
	beep(349, 463 / 50);
	beep(392, 187 / 50);
	beep(440, 750 / 50);
	beep(494, 375 / 50);
	beep(523, 375 / 50);
	beep(587, 1125 / 50);
}

// Comando que ejecuta la excepcion Invop
void commandInvOp()
{
	clearScreen();
	INVALID_OP_CODE();
}

// Imprime que hacer para salir de la aplicacion
void printExitHelp()
{
	appendstring("Presione ");
	printColor("'ESC'", 0xE9AD0C, 0);
	print(" para volver a la consola.\n", 0);
}

// Función cat que imprime lo que se escribe por stdin
static void *catpro(int argc, char *argv[])
{
	char c = 1;
	while (c != (char)EOF)
	{
		c = getchar();
		if (isalnum(c) || '\n' == c || ' ' == c || c == EOF)
		{
			print("%c", c);
		}
	}
	exit();
	return NULL;
}

// Comando que ejecuta cat
void commandCat(char *str)
{
	print("\n");
	printColor("> ", 0x547891);
	int catpid = createFGProcess("cat", catpro, 0, NULL);
	waitPid(catpid);
	return;
}

// Funcion wc que cuenta lineas de stdin
static void *wcpro(int argc, char *argv[])
{
	int lineCount = 0;
	char c = 1;
	int isNewline = 0;
	while (c != (char)EOF)
	{
		c = getchar();
		if (c == '\n')
		{
			isNewline = 1;
			lineCount++;
		}
		else
		{
			isNewline = 0;
		}
		if (isalnum(c) || '\n' == c || ' ' == c)
		{
			print("%c", c);
		}
	}
	if (!isNewline)
	{
		lineCount++;
	}

	print("\nLines: %d\n", lineCount);

	exit();
	return NULL;
}

// Comando que ejecuta wc
void commandWc(char *str)
{
	print("\n");
	printColor("> ", 0x547891);
	int wcpid = createFGProcess("wc", wcpro, 0, NULL);
	waitPid(wcpid);
}

// Funcion filter que imprime lo que se escribe por stdin sin vocales
static void *filterpro(int argc, char *argv[])
{

	char c = 1;
	while (c != (char)EOF)
	{
		c = getchar();
		char lowerC = tolower(c);
		if (lowerC != 'a' && lowerC != 'e' && lowerC != 'i' && lowerC != 'o' && lowerC != 'u')
		{
			print("%c", c);
		}
	}
	exit();
	return NULL;
}

// Comando que ejecuta filter
void commandFilter(char *str)
{
	print("\n");
	printColor("> ", 0x547891);
	int filterpid = createFGProcess("filter", filterpro, 0, NULL);
	waitPid(filterpid);
	return;
}

// Comando para matar un proceso con pid en string
void commandKill(char *str)
{
	/* if (streql(str, "ALL"))
	{
		for (int i = 0; i < MAX_PID; i++)
		{
			int pid = kill(i);
			if (pid == -1)
			{
				print("No se pudo matar el proceso con pid %d\n", i);
			}
			else
			{
				print("Se mato el proceso con pid %d\n", pid);
			}
		}
	} */
	if (str[0] != 0)
	{
		int pid = strToInt(str);
		if (pid < 0 || pid > MAX_PID)
		{
			print("PID invalido\n");
			return;
		}
		int i = kill(pid); // intento matar
		if (i == -1)
		{
			print("No se pudo matar el proceso con pid %d\n", pid);
		}
		else
		{
			print("Se mato el proceso con pid %d\n", pid);
		}
	}
}

// Comando para cambiar la prioridad de un proceso con pid en string y prioridad en string
void commandNice(char *pid, char *priority)
{
	int pidInt = strToInt(pid);
	int priorityInt = strToInt(priority);
	if (pidInt < 0 || pidInt > MAX_PID)
	{
		print("PID invalido\n");
		return;
	}
	if (priorityInt < 0 || priorityInt > 4)
	{
		print("Prioridad invalida\n");
		return;
	}
	int ans = nice(pidInt, priorityInt);
	if (ans == -1)
	{
		print("No se pudo cambiar la prioridad del proceso con pid %d\n", pidInt);
	}
	else if (ans == 0)
	{
		print("El proceso con pid %d ya tiene prioridad %d \n", pidInt, priorityInt);
	}
	else
	{
		print("Se cambio la prioridad del proceso con pid %d a %d\n", pidInt, priorityInt);
	}
}

// Comando que bloquea un proceso con pid en string
void commandBlock(char *pid)
{
	int pidInt = strToInt(pid);
	if (pidInt < 0 || pidInt > MAX_PID)
	{
		print("PID invalido\n");
		return;
	}
	int ans = block(pidInt);
	if (ans == -1)
	{
		print("No se pudo bloquear el proceso con pid %d\n", pidInt);
	}
	else
	{
		print("Se bloqueo el proceso con pid %d\n", pidInt);
	}
}

// comando que desbloquea un proceso con pid en string
void commandUnblock(char *pid)
{
	int pidInt = strToInt(pid);
	if (pidInt < 0 || pidInt > MAX_PID)
	{
		print("PID invalido\n");
		return;
	}
	int ans = unblock(pidInt);
	if (ans == -1)
	{
		print("No se pudo desbloquear el proceso con pid %d\n", pidInt);
	}
	else
	{
		print("Se desbloqueo el proceso con pid %d\n", pidInt);
	}
}

// Funcion para el testeo de wait
void *timmy(int argc, char **argv)
{
	print("-Tommy: Dad, Ill take a nap in my last seconds of life\n");
	makeshiftSleep(60);
	print("*peacefully dies in his sleep*\n");
	exit();
	return NULL;
}

// Funcion para el testeo de wait
static void *dad(int argc, char **argv)
{
	makeshiftSleep(20);
	print("\n-Dad: Please Timmy! Please Dont Die On Me!\n");
	makeshiftSleep(20);
	int timmyPID = createFGProcess("timmy", timmy, 0, NULL);
	waitPid(timmyPID); // estoy esperando a la llegada de este pid
	makeshiftSleep(20);
	print("-Dad: F in the chat\n");
	exit();
	return NULL;
}

// Funcion que imprime infinitamente el pid del proceso
void *commandLoop(int argc, char **argv)
{
	int currentPid = getPid();
	while (1)
	{
		print("cuchau PID: %d\n", currentPid);

		// Sleep for 1 second (assuming gettick() returns time in milliseconds)
		makeshiftSleep(30);
	}
	exit();
	return NULL;
}

// Comando que ejecuta la aplicacion del dilema de los filosofos
void commandPhylo()
{
	int pidPhylo = createFGProcess("phylo", phyloProcess, 0, NULL);
	waitPid(pidPhylo);
	return;
}

// Comando que imprime el estado de la memoria
void commandPrintMemState(int unit)
{
	char s[1000];
	mem(s, unit);
	print("%s", s);
}

// Comando que imprime los procesos
void commandPrintProcesses()
{
	char s[1000];
	ps(s);
	// int n = strlen(s);
	print("%s", s);
}

// Comando de testeos de semaforos
void testSemaphoresSync()
{
	int argc = 3;
	char *argv[] = {"10", "1", "0"};
	test_sync(argc, argv);
	return;
}

// Comando de testeos de semaforos sin sincronizacion
void testSemaphoresNoSync()
{
	int argc = 3;
	char *argv[] = {"10", "1", "1"};
	test_sync(argc, argv);
	return;
}

// Carga al historial el comando s
static void loadHistory(const char *s)
{
	int len = strlen(s);
	if (historyDim > 0 && strcmp(historyBuffer[(historyDim - 1) % MAX_COMMANDS], s) == 0)
	{
		return;
	}
	strncpy(historyBuffer[historyDim % MAX_COMMANDS], s, MAX_COMMAND_LENGTH - 1);
	historyBuffer[historyDim % MAX_COMMANDS][len] = '\0';
	historyDim++;
	historyIndex = historyDim;
}

// Sube en el historial a mas antiguos
static char *upHistory()
{
	if (historyIndex > 0)
	{
		if (historyIndex == historyDim)
		{
			return historyBuffer[--historyIndex % MAX_COMMANDS];
		}
		historyIndex--;
		return historyBuffer[historyIndex % MAX_COMMANDS];
	}
	beep(100, 1);
	return "";
}

// Baja en el historial a mas nuevos
static char *downHistory()
{
	if (historyIndex < historyDim)
	{
		historyIndex++;
		if (historyIndex == historyDim)
		{
			clearconsoleBuffer();
			return "";
		}
		return historyBuffer[historyIndex % MAX_COMMANDS];
	}
	beep(100, 1);
	return "";
}

// Resetea la linea de comando
static void inLineReset()
{
	for (int i = 0; i < lastChar; i++)
	{
		backspace();
	}
	lastChar = 0;
	clearconsoleBuffer();
}

// imprime el historial
void printHistory()
{
	for (int i = 0; i < historyDim; i++)
	{
		printColor("%s\n", FONTCOLOR, historyBuffer[i]);
	}
}

// Funcion que ejecuta up o down de las felchas
void upArrow(int arrowUp)
{
	inLineReset();
	char *aux;
	if (arrowUp)
	{
		if (historyIndex > 0)
		{
			aux = upHistory();
		}
		else
		{
			return;
		}
	}
	else
	{
		aux = downHistory();
	}
	printColor("%s", FONTCOLOR, aux);
	strncpy(consoleBuffer, aux, MAX_COMMAND_LENGTH - 1);
	lastChar = strlen(consoleBuffer);
}

// Funcion resetea historial
void clearHistoryBuffer()
{
	for (int i = 0; i < MAX_COMMANDS; i++)
	{
		for (int j = 0; j < MAX_COMMAND_LENGTH; j++)
		{
			historyBuffer[i][j] = 0;
		}
	}
	historyDim = historyIndex = 0;
}

// Funcion que maneja las teclas
void handleKey(char c)
{
	switch (c)
	{
	case BACKSPACE:
	{
		if (lastChar != 0)
		{
			backspace();
			consoleBuffer[--lastChar] = 0;
		}
		else
		{
			// beep(80, 1);
		}
		break;
	}
	case ESC:
	{
		clearScreen();
		clearconsoleBuffer();
		clearHistoryBuffer();
		restartCursor();
		printColor("user@Qemu:", USER_TEXT_COLOR, 0);
		printColor("> $ ", TERMINAL_BLUE, 0);

		break;
	}
	case ENTER:
	{
		newline();
		if (lastEnter())
		{
			// scrollUp(32, TERMINAL_BLUE);
			clearScreen();
		}

		if (consoleBuffer[0])
		{
			historyIndex = historyDim;
			loadHistory(consoleBuffer);
			handleCommand(consoleBuffer);
			clearconsoleBuffer();
		}
		printColor("user@Qemu:", USER_TEXT_COLOR, 0);
		printColor("> $ ", TERMINAL_BLUE, 0);
		break;
	}

	case '\t':
	{
		appendstring("    ");
		for (int i = 0; i < 4; i++)
		{
			consoleBuffer[lastChar + i] = ' ';
		}
		lastChar += 4;
		break;
	}
	case ASC_UP:
	{ // up arrow key ascii
		upArrow(1);
		break;
	}
	case ASC_DOWN:
	{ // down arrow key ascii

		upArrow(0);
		break;
	}
	case ':':
	{					  // right arrow key ascii
		storeRegisters(); // saca el screenshot de los registros
		break;
	}
	case EOF:
	{
		appendstringColor("^D", 0xFF0303);
		for (int i = 0; i < 2; i++)
		{
			consoleBuffer[lastChar + i] = ' ';
		}
		lastChar += 2;
		break;
	}
	case KILL_PROCESS:
	{
		appendstringColor("^C", 0xFF0303);
		for (int i = 0; i < 2; i++)
		{
			consoleBuffer[lastChar + i] = ' ';
		}
		lastChar += 2;
		break;
	}
	default:
	{
		if (!canWrite())
		{
			break;
		}
		if (lastChar < MAX_CONSOLE_BUFFER)
		{
			consoleBuffer[lastChar++] = c;
			putchar(c);
		}
		else
		{
			beep(80, 1);
		}

		break;
	}
	}
}

// Funciont que maneja los procesos en background
void handleBGProcess()
{
	char leftCommand[128] = {0};
	char leftSection[128] = {0};

	splitString(consoleBuffer, leftCommand, '&');
	strcpy(leftCommand, consoleBuffer);
	splitString(leftCommand, leftSection, ' ');

	toUpper(leftCommand);

	if (strcmp(leftCommand, "LOOP") == 0)
	{
		createBGProcess("loop", commandLoop, 0, NULL);
	}
	else if (strcmp(leftCommand, "HELP") == 0)
	{
		createBGProcess("help", commandHelp, 0, NULL);
	}
	else if (strcmp(leftCommand, "CAT") == 0)
	{
		createBGProcess("cat", catpro, 0, NULL);
	}
	else if (strcmp(leftCommand, "FILTER") == 0)
	{
		createBGProcess("filter", filterpro, 0, NULL);
	}
	else if (strcmp(leftCommand, "WC") == 0)
	{
		createBGProcess("wordcount", wcpro, 0, NULL);
	}
}

// Funcion que maneja los pipes
void handlePipe()
{
	char leftCommand[128] = {0};
	char rightCommand[128] = {0};
	char leftSection[128] = {0};
	char rightSection[128] = {0};

	splitString(consoleBuffer, rightCommand, '|');

	strcpy(leftCommand, consoleBuffer);

	splitString(leftCommand, leftSection, ' ');
	while (rightCommand[0] == ' ')
	{
		strcpy(rightCommand, rightCommand + 1);
	}
	splitString(rightCommand, rightSection, ' ');
	toUpper(leftCommand);
	toUpper(rightCommand);

	int pipeFD = openPipe("superpipe");
	int writeFD[] = {0, pipeFD};
	int readFD[] = {pipeFD, 1};

	int writerPID = -1;
	int readerPID = -1;

	switch (hash(leftCommand))
	{
	case HELP:
		writerPID = createProcess("helpc", commandHelp, 0, NULL, writeFD);
		waitPid(writerPID);

		break;
	case CAT:
		writerPID = createProcess("catc", catpro, 0, NULL, writeFD);
		waitPid(writerPID);

		break;
	case FILTER:
		writerPID = createProcess("filterc", filterpro, 0, NULL, writeFD);
		waitPid(writerPID);
		break;
	case WC:
		writerPID = createProcess("wcc", wcpro, 0, NULL, writeFD);
		waitPid(writerPID);
		break;
	default:
		print("%s : command not found.\n", leftCommand);
		closePipe(pipeFD);
		return;
		break;
	}

	print("\n------------------------\n");

	switch (hash(rightCommand))
	{
	case HELP:
		readerPID = createProcess("helpc", commandHelp, 0, NULL, readFD);
		waitPid(readerPID);
		break;
	case CAT:
		readerPID = createProcess("catc", catpro, 0, NULL, readFD);
		waitPid(readerPID);
		break;
	case FILTER:
		readerPID = createProcess("filterc", filterpro, 0, NULL, readFD);
		waitPid(readerPID);

		break;
	case WC:
		readerPID = createProcess("wcc", wcpro, 0, NULL, readFD);
		waitPid(readerPID);
		break;
	default:
		print("%s : command not found.\n", rightCommand);
		closePipe(pipeFD);

		break;
	}
}

// Testea el memory manager
void testMemoryManager()
{
	char *argv[] = {"134217728"};
	test_mm(1, argv);
	return;
}

// Testea el scheduler
static void testPrio()
{
	print("TESTING PRIORITY\n");
	test_prio();
	print("TEST ENDED\n");
}

// Testea los procesos
static void *testProc(int argc, char **argv)
{
	print("TESTING PROCESSES\n");
	char *testargv[] = {"10"};
	test_processes(1, testargv);
	print("TEST ENDED\n");
	exit();
	return NULL;
}

// Maneja los comandos regulares
void handleRegularCommand()
{
	char section[128] = {0};
	char *command = toUpper(consoleBuffer);
	splitString(command, section, ' ');
	int hashedCommand = hash(command);
	int commandPID;
	// int hashedSection = hash(section);
	if (section[0] == 0)
	{
		switch (hashedCommand)
		{
		case HELP:
			commandPID = createFGProcess("help", commandHelp, 0, NULL);
			waitPid(commandPID);
			break;
		case TRON:
			commandTron();
			break;
		case CLEAR:
			commandClear();
			break;
		case BEEP:
			commandBeep();
			break;
		case ANTHEM:
			commandAnthem();
			break;
		case INFOREG:
			commandInforeg();
			break;
		case TIME:
			commandTime();
			break;
		case INVOP:
			commandInvOp();
			break;
		case DIVCERO:
			commandDivCero();
			break;
		case PIANO:
			commandPiano();
			break;
		case UPSIZE:
			clearScreen();
			changeFontSize(1);
			break;
		case DOWNSIZE:
			clearScreen();
			changeFontSize(-1);
			break;
		case LANG_ES:
			changelanguage(1);
			break;
		case LANG_EN:
			changelanguage(0);
			break;
		case MEM:
			commandPrintMemState(0);
			break;
		case MEMB:
			commandPrintMemState(1);
			break;
		case PS:
			commandPrintProcesses();
			break;
		case LOOP:
			commandPID = createFGProcess("loop", commandLoop, 0, NULL);
			waitPid(commandPID);
			break;
		case TEST_MM:
			testMemoryManager();
			break;
		case TEST_SYNC:
			testSemaphoresSync();
			break;
		case TEST_NO_SYNC:
			testSemaphoresNoSync();
			break;
		case TEST_WAIT:
			commandPID = createFGProcess("dad", dad, 0, NULL);
			waitPid(commandPID);
			break;
		case TEST_PRIO:
			testPrio();
			break;
		case TEST_PROC:
			createFGProcess("testProcess", testProc, 0, NULL);
			; // poponeta
			break;
		case CAT:
			commandCat(section);
			break;
		case WC:
			commandWc(section);
			break;
		case FILTER:
			commandFilter(section);
			break;
		case PHYLO:
			commandPhylo();
			break;
		default:
			printColor("'%s'", ORANGY, command);
			print(" : comando no encontrado.\n");
			break;
		}
	}
	else // composed commands
	{
		switch (hashedCommand)
		{
		case KILL:
			commandKill(section);
			break;
		case NICE:
		{
			char priority[64] = {0};
			splitString(section, priority, ' ');
			commandNice(section, priority);
			break;
		}
		case BLOCK:
			commandBlock(section);
			break;
		case UNBLOCK:
			commandUnblock(section);
			break;
		case MEMACCESS:
			commandMemAccess(section);
			break;
		default:
			printColor("'%s ...'", ORANGY, command);
			print(" : comando no encontrado.\n");
			break;
		}
	}
}

// Maneja los comandos
void handleCommand()
{
	if (hasPipe(consoleBuffer))
	{
		handlePipe();
		return;
	}
	if (hasAmpersand(consoleBuffer))
	{
		handleBGProcess();
		return;
	}
	else
	{
		handleRegularCommand();
		return;
	}
}
