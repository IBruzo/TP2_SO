#include <shell.h>

void *writer(int argc, char **argv)
{
	print("Esto fue leido de un pipe buffer!\n");
	exit();
	return NULL;
}

void *reader(int argc, char **argv)
{
	print("Voy a leer de un buffer! \n ");
	char c;
	while ((c = getchar()) != 0)
	{
		print("%c", c); // escribe en consola
	}
	print("Reading finished");

	exit();
	return NULL;
}

void *sleepyJoe(int argc, char **argv)
{
	print("\nIm going to sleep 5 seconds, like Timmy...\n");
	sleep(5);
	print("Im waking up, I FEEL IT IN MY BONES\n");
	exit();
	return NULL;
}

void *initialize_shell(int argc, char **argv)
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	printColor("user@Qemu:", USER_TEXT_COLOR, 0);
	printColor("> $ ", TERMINAL_BLUE, 0);

	//	createFGProcess("sleepyJoe", sleepyJoe, 0, NULL );

	/* --------------------------PIPE TEST---------------------------- */

	// // writer | reader
	// int pipeFD = openPipe("superpipe");
	// print("PIPE FD = %d\n", pipeFD);
	// int writeFD[] = {0, pipeFD};
	// int readFD[] = {pipeFD, 1};

	// int writerPID = createProcess("writer", writer, 0, NULL, writeFD);
	// print("[[%d]]", writerPID);
	// waitPid(writerPID);
	// int readerPID = createProcess("reader", reader, 0, NULL, readFD);
	// waitPid(readerPID);
	// closePipe(pipeFD);
	// print("pipe closed");

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
