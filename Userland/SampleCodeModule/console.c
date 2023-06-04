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

void commandHelp()
{
	clearScreen();
	restartCursor();
	printColor("BIENVENIDO AL MENU HELP", 0xE9AD0C, 0);
	newline();
	appendstring("EL SISTEMA CUENTA CON LOS SIGUIENTES COMANDOS: <User commands>");
	newline();
	appendstring("- HELP");
	newline();
	appendstring("- TRON");
	newline();
	appendstring("- INFOREG");
	newline();
	appendstring("- MEMACCESS ( MEM ADDRESS )");
	newline();
	printColor("- INVOP", RED, 0);
	newline();
	printColor("- DIVCERO", RED, 0);
	newline();
	appendstring("- BEEP");
	newline();
	appendstring("- TIME");
	newline();
	appendstring("- SIZE(+ o -)");
	newline();
	printColor("- ANTHEM", TERMINAL_BLUE, 0);
	newline();
	printColor("- CLEAR", TERMINAL_BLUE, 0);
	newline();
	printColor("- PIANO", TERMINAL_BLUE, 0);
	newline();
	printColor("- LANGUAGE=(EN o ES) ", TERMINAL_BLUE, 0);
	newline();
	printColor("Para mas informacion, ingrese HELP <command>.\n", 0xE9AD0C, 0);
	appendstring("Presione ");
	printColor("'ESC'", 0xE9AD0C, 0);
	print(" para volver a la consola.\n", 0);
	waitForKey(ESC);
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
void testMemoryManager()
{
	char *argv[] = {"134217728"};
	test_mm(1, argv);
	return;
}
void testSemaphoresSync()
{
	test_sync();
	return;
}

void *catpro(int argc, char *argv[])
{
	clearScreen();
	printColor("write here > ", 0x547891);
	char c = 1;
	char buff[100];
	int lastch = 0;
	while (c != (char)EOF)
	{
		c = getchar();
		if (c == '\n')
		{
			newline();
			buff[lastch] = '\0';
			printColor("%s\n", 0x547891, buff);
			lastch = 0;
			printColor("-", 0x547891);
		}
		else if (c != 0)
		{
			handleKey(c);
			buff[lastch++] = c;
		}
	}
	newline();
	exit();
	return NULL;
}

void commandCat(char *str)
{

	int catpid = createFGProcess("cat", catpro, 0, NULL);
	waitPid(catpid);
	return;
}
static void *wcpro(int argc, char *argv[])
{
	clearScreen();
	printColor("edit here>", 0x547891);
	char c = 1;
	char buff[100];
	int lastch = 0;
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

			lineCount++;
			newline();
		}
		else if (c == ' ')
		{
			if (inWord)
			{
				wordCount++;
				inWord = 0;
			}
		}
		else
		{
			if (!inWord)
			{
				inWord = 1;
			}
		}

		if (c == '\n')
		{
			buff[lastch] = '\0';
			printColor("%s\n", 0x547891, buff);
			lastch = 0;
			printColor(">", 0x547891);
		}
		else if (c != 0)
		{
			handleKey(c);
			buff[lastch++] = c;
		}
	}

	newline();
	printColor("Lines: %d\n", 0x547891, lineCount);
	printColor("Words: %d\n", 0x547891, wordCount);
	printColor("Characters: %d\n", 0x547891, charCount);

	exit();
	return NULL;
}

static void *filterpro(int argc, char *argv[])
{
	clearScreen();
	printColor("edit here>", 0x547891);
	char c = 1;
	char buff[100];
	int lastch = 0;
	while (c != (char)EOF)
	{
		c = getchar();
		if (c == '\n')
		{
			newline();
			buff[lastch] = '\0';
			printColor("%s\n", 0x547891, buff);
			lastch = 0;
			printColor(">", 0x547891);
		}
		else if (c != 0)
		{
			// Check if c is a vowel (case-insensitive)
			char lowerC = tolower(c);
			handleKey(c);
			if (lowerC != 'a' && lowerC != 'e' && lowerC != 'i' && lowerC != 'o' && lowerC != 'u')
			{
				buff[lastch++] = c;
			}
		}
	}
	newline();
	exit();
	return NULL;
}

void commandFilter(char *str)
{
	int filterpid = createFGProcess("filter", filterpro, 0, NULL);
	waitPid(filterpid);
	return;
}

void commandWc(char *str)
{
	int wcpid = createFGProcess("wc", wcpro, 0, NULL);
	waitPid(wcpid);
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
	print("-Tommy: Dad, Ill take a nap in my last 5 seconds of life\n");
	sleep(5);
	print("*peacefully dies in his sleep*\n");
	exit();
	return NULL;
}

static void *dad(int argc, char **argv)
{
	sleep(1);
	print("\n-Dad: Please Timmy! Please Dont Die On Me!\n");
	sleep(1);
	int timmyPID = createFGProcess("timmy", timmy, 0, NULL);
	print("(( Timmy PID [%d] ))\n", timmyPID);
	waitPid(timmyPID); // estoy esperando a la llegada de este pid
	sleep(1);
	print("-Dad: F in the chat\n");
	exit();
	return NULL;
}

void commandLoop()
{
	int currentPid = getPid();
	while (1)
	{
		print("cuchau PID: %d\n", currentPid);
		sleep(1);
	}
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

static void testWait()
{
	createFGProcess("dad", dad, 0, NULL);
	handleKey(ENTER);
}
static void testPrio()
{
	print("TESTING PRIORITY\n");
	test_prio();
	print("TEST ENDED\n");
}

static void testProcesses()
{
	print("TESTING PROCESSES\n");
	char *argv[] = {"10"};
	test_processes(1, argv);
	print("TEST ENDED\n");
}
// CHEQUEAR CUAL ES EL COMANDO Y QUE EL COMANDO EXISTA CON LOS HASHCODES
void handleCommand()
{
	if (hasPipe(consoleBuffer))
	{
		handlePipe();
		return;
	}
	else
	{
		handleRegularCommand();
		return;
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

	// leftCommand = toUpper(leftCommand);
	// rightCommand = toUpper(rightCommand);
}

void handleRegularCommand()
{
	char section[128] = {0};
	char *command = toUpper(consoleBuffer);
	splitString(command, section, ' ');
	int hashedCommand = hash(command);
	// int hashedSection = hash(section);
	if (section[0] == 0)
	{
		switch (hashedCommand)
		{
		case HELP:
			commandHelp();
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
			commandLoop();
			break;
		case TEST_MM:
			testMemoryManager();
			break;
		case TEST_SYNC:
			testSemaphoresSync();
			break;
		case TEST_WAIT:
			testWait();
			break;
		case TEST_PRIO:
			testPrio();
			break;
		case TEST_PROC:
			testProcesses();
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
		case HELP:
			handleHelp(section);
			break;

		default:
			printColor("'%s'", ORANGY, command);
			print(" : comando no encontrado.\n");
			break;
		}
	}
}

void handleHelp(char *helping)
{
	int hashedHelp = hash(helping);
	clearScreen();
	restartCursor();
	switch (hashedHelp)
	{
	case HELP:
	{
		appendstring("En serio?\n\n");
		printColor("HELP: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Ayuda.\n");
		appendstring("De no ser seguido por nada, lista los comandos disponibles.\n");
		appendstring("De ser seguido por un comando, da informacion sobre el mismo.\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case TRON:
	{
		printColor("TRON: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Juego de dos jugadores, al mejor de 5.\n");
		appendstring("Pierde quien choque contra si mismo,\nel otro jugador, o los bordes.\n");
		appendstring("El jugador 1 se mueve con las flechas.\n");
		appendstring("El jugador 2 se mueve con las teclas W,A,S y D.\n");
		appendstring("Sorpresa al final :)\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case BEEP:
	{
		printColor("BEEP: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Emite un beeeeep\n\n");
		beep(440, 1);
		printExitHelp();
		waitForKey(ESC);
	}
	break;
	case ANTHEM:
	{
		printColor("ANTHEM: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("El himno de mi corazon, \npero no el de los abuelos de la nada.\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case INFOREG:
	{
		printColor("INFOREG: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Imprime en pantalla el valor de los registros\nal momento de ejecucion.\n");
		appendstring("Para realizar una toma de estado, presione ':' .\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case TIME:
	{
		printColor("TIME: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("De encontrarse en una isla desierta \nsin medios para saber la hora,\nuse este comando\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case INVOP:
	{
		printColor("INVOP: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Envia un codigo de operacion invalido.\n");
		appendstring("Que puede salir mal?\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case DIVCERO:
	{
		printColor("DIVCERO: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Divide por cero.\n");
		appendstring("Totalmente inocuo.\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case PIANO:
	{
		printColor("PIANO: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Componga su proxima obra maestra.\n");
		appendstring("Piano de eleccion de: \nPablo Lescano, Paul McCartney y Wolfgang Amadeus Mozart.\n");
		appendstring("Blancas: ASDFGHJ\n");
		appendstring("Negras: WETYUOP\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case MEMACCESS:
	{
		printColor("MEMACCESS (MEM ADRESS): <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Imprime por pantalla los 32 bits posteriores\na la direccion de memoria especificada.\n");
		appendstring("Debe ser ingresada la zona de memoria a acceder luego del comando separado por una espacio.\n");
		appendstring("Modo de uso: <memaccess [direccion]>\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case CLEAR:
	{
		printColor("CLEAR: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Limpia la pantalla.\n");
		appendstring("AUSPICIADO POR MR MUSCULO\n");
		appendstring("ANIQUILA LOS GERMENES, SUFRIMIENTO MAXIMO\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case LANGUAGE:
	{
		printColor("LANGUAGE: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Cambia el teclado.\n");
		appendstring("Seguido por '=es' -> espa~ol\n");
		appendstring("Seguido por '=en' -> ingles\n");
		appendstring("Modo de uso <language=es> o <language=en> \n");
		appendstring("Ruso y Klingon disponibles proximamente.\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	case SIZE:
	{
		printColor("SIZE: <User Commands>\n\n", 0x0F66151, 0);
		appendstring("Agrande o achique el tama~o del texto en pantalla \nsiguiendo el comando con un + o un -,\nrespectivamente.\n");
		appendstring("Modo de uso: <size+>\n\n");
		printExitHelp();
		waitForKey(ESC);
		break;
	}
	default:

		break;
	}
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
		appendstringColor("EOF", 0xFF0303);
		for (int i = 0; i < 3; i++)
		{
			consoleBuffer[lastChar + i] = -1;
		}
		lastChar += 3;
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
