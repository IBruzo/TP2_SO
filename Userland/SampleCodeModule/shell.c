#include <shell.h>

void *writer(int argc, char **argv)
{
	print("Im writing to a Buffer :D\n");
	exit();
	return NULL;
}

void *reader(int argc, char **argv)
{
	while (1)
	{
		char c = getchar(); // lee del pipe
		print("%c", c);		// escribe en consola
	}
	return NULL;
}

void *initialize_shell(int argc, char **argv)
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	printColor("user@Qemu:", USER_TEXT_COLOR, 0);
	printColor("> $ ", TERMINAL_BLUE, 0);

	/* --------------------------PIPE TEST---------------------------- */

	/*
	esto seria MAS O MENOS el testeo del pipe, las syscalls de semaforos
	que usa internamente el pipe tienen que ser actualizadas
	*/

	// pipeo de procesos reader y writers int pipeFD = openPipe("superpipe");
	// int writeFD[] = {0, pipeFD};
	// int readFD[] = {pipeFD, 1};
	//
	// int writerPID = createProcess(writer, 0, NULL, writeFD);
	// int readerPID = createProcess(reader, 0, NULL, readFD);
	//
	// waitPid(readerPID);
	// closePipe(pipeFD);

	/* ---------------------------------------------------------------- */

	while (1)
	{
		char c = getchar();
		if (c != 0)
		{
			handleKey(c);
		}
	}
	return NULL;
}
