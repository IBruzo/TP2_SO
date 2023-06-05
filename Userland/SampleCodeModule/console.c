// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <console.h>
#include "tests.h"

#define MAX_PID 5000
void handleHelp(char *helping);

extern void INVALID_OP_CODE();
static char consoleBuffer[MAX_CONSOLE_BUFFER] = {0};
static int lastChar = 0;

char historyBuffer[MAX_COMMANDS][MAX_COMMAND_LENGTH];
unsigned int historyIndex = 0;
unsigned int historyDim = 0;

// pasa a mayusculas
char *toUpper(char *string)
{
	int i = 0;
	while (string[i] != 0)
	{
		if (string[i] >= 'a' && string[i] <= 'z')
			string[i] = string[i] - 32;

		i++;
	}
	return string;
}

// funcion que convierte un bcd en decimal
static int bcdToDec(int bcd)
{
	return ((bcd / 16) * 10 + (bcd % 16));
}

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

void clearconsoleBuffer()
{
	for (int i = 0; i < lastChar; i++)
		consoleBuffer[i] = 0;
	lastChar = 0;
}

void clearScreen()
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	restartCursor();
}

// pequeña funcion que espera a que se presione una tecla para salir
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
	print("\n");
	exit();
	return NULL;
}

void commandClear()
{
	clearScreen();
	restartCursor();
}

void commandSnapshot()
{
	clearScreen();
	getRegisters();
	waitForKey(ESC);
}

void commandTime()
{
	printCurrentTime();
}

void commandTron()
{
	playTron();
	restartCursor();
	clearScreen();
}

void commandPiano()
{
	print("Usted se encuentra frente a un teclado especial.\n\n", 0);
	print("Presione teclas...\n\n", 0);
	appendstring("Para salir presione la 'SPACE' bar.\n\n");
	playPiano();
	commandClear();
}

void commandBeep()
{
	beep(1000, 10);
}

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

void commandDivCero()
{
	clearScreen();
	int x = 1;
	int y = 0;
	int z = x / y;
	while (z)
		break;
}

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

void commandInvOp()
{
	clearScreen();
	INVALID_OP_CODE();
}

// https://stackoverflow.com/questions/4014827/how-can-i-compare-strings-in-c-using-a-switch-statement
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

void printExitHelp()
{
	appendstring("Presione ");
	printColor("'ESC'", 0xE9AD0C, 0);
	print(" para volver a la consola.\n", 0);
}

static void *catpro(int argc, char *argv[])
{
	char c = 1;

	while (c != (char)EOF)
	{
		c = getchar();
		if (c == '\n')
		{
			print("\n");
		}
		else if (c != 0)
		{
			print("%c", c);
		}
	}

	newline();
	exit();
	return NULL;
}

void commandCat(char *str)
{
	clearScreen();
	printColor("> ", 0x547891);
	int catpid = createFGProcess("cat", catpro, 0, NULL);
	waitPid(catpid);
	return;
}

static void *wcpro(int argc, char *argv[])
{

	char c = 1;

	int lineCount = 0;
	int wordCount = 0;
	int charCount = 0;
	int inWord = 0;

	while (c != (char)EOF)
	{
		c = getchar();
		if (isalnum(c))
		{
			charCount++;
		}

		if (c == '\n')
		{
			wordCount++;
			newline();
		}
		else if (c == ' ')
		{
			handleKey(c);
			if (inWord)
			{
				wordCount++;
				inWord = 0;
			}
		}
		else if (c != 0)
		{
			handleKey(c);
		}
		else
		{
			if (!inWord)
			{
				inWord = 1;
			}
		}
	}
	printColor("Lines: %d\n", 0x547891, lineCount + 1);
	printColor("Words: %d\n", 0x547891, wordCount + 1);
	printColor("Characters: %d\n", 0x547891, charCount);
	exit();
	return NULL;
}

void commandWc(char *str)
{
	clearScreen();
	printColor("> ", 0x547891);
	int wcpid = createFGProcess("wc", wcpro, 0, NULL);
	waitPid(wcpid);
}

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

	newline();
	exit();
	return NULL;
}

void commandFilter(char *str)
{
	clearScreen();
	printColor("> ", 0x547891);
	int filterpid = createFGProcess("filter", filterpro, 0, NULL);
	waitPid(filterpid);
	return;
}

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

void *timmy(int argc, char **argv)
{
	print("-Tommy: Dad, Ill take a nap in my last seconds of life\n");
	makeshiftSleep(60);
	print("*peacefully dies in his sleep*\n");
	exit();
	return NULL;
}

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

void commandPrintMemState(int unit)
{
	char s[1000];
	mem(s, unit);
	print("%s", s);
}

void commandPrintProcesses()
{
	char s[1000];
	ps(s);
	// int n = strlen(s);
	print("%s", s);
}

// CARGA AL HISTORIAL DE COMANDOS
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

// SUBE EN EL HISTORIAL A MAS VIEJOS
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

// BAJA EN EL HISTORIAL A MAS RECIENTES
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

// RESETEA LA LINEA
static void inLineReset()
{
	for (int i = 0; i < lastChar; i++)
	{
		backspace();
	}
	lastChar = 0;
	clearconsoleBuffer();
}

void printHistory()
{
	for (int i = 0; i < historyDim; i++)
	{
		printColor("%s\n", FONTCOLOR, historyBuffer[i]);
	}
}

// FUNCION QUE EJECUTA UP OR DOWN
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

// RESETEA EL HISTORIAL
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

// SE FIJA QUE TECLA HA SIDO ACCINOADA Y QUE HACER AL RESPECTO...
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

	if (strcmp(leftCommand, "HELP") == 0)
	{
		writerPID = createProcess("command1", commandHelp, 0, NULL, writeFD);
		waitPid(writerPID);
	}
	else if (strcmp(leftCommand, "CAT") == 0)
	{
		writerPID = createProcess("command1", catpro, 0, NULL, writeFD);
		waitPid(writerPID);
	}
	else if (strcmp(leftCommand, "FILTER") == 0)
	{
		writerPID = createProcess("command1", filterpro, 0, NULL, writeFD);
		waitPid(writerPID);
	}
	else if (strcmp(leftCommand, "WC") == 0)
	{
		writerPID = createProcess("command1", wcpro, 0, NULL, writeFD);
		waitPid(writerPID);
	}

	print("------------------------\n");

	if (strcmp(rightCommand, "HELP") == 0)
	{
		readerPID = createProcess("command2", commandHelp, 0, NULL, readFD);
		waitPid(readerPID);
	}
	else if (strcmp(rightCommand, "CAT") == 0)
	{
		readerPID = createProcess("command2", catpro, 0, NULL, readFD);
		waitPid(readerPID);
	}
	else if (strcmp(rightCommand, "FILTER") == 0)
	{
		readerPID = createProcess("command2", filterpro, 0, NULL, readFD);
		waitPid(readerPID);
	}
	else if (strcmp(rightCommand, "WC") == 0)
	{
		readerPID = createProcess("command2", wcpro, 0, NULL, readFD);
		waitPid(readerPID);
	}

	closePipe(pipeFD);
}

void testMemoryManager()
{
	char *argv[] = {"134217728"};
	test_mm(1, argv);
	return;
}

static void testPrio()
{
	print("TESTING PRIORITY\n");
	test_prio();
	print("TEST ENDED\n");
}

static void *testProc(int argc, char **argv)
{
	print("TESTING PROCESSES\n");
	char *testargv[] = {"10"};
	test_processes(1, testargv);
	print("TEST ENDED\n");
	exit();
	return NULL;
}

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
			commandSnapshot();
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
			commandPID = createFGProcess("testSync", test_sync, 0, NULL);
			waitPid(commandPID);
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

// CHEQUEAR CUAL ES EL COMANDO Y QUE EL COMANDO EXISTA CON LOS HASHCODES
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
