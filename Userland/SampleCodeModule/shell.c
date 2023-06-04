// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>

void *initialize_shell(int argc, char **argv)
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	printColor("user@Qemu:", USER_TEXT_COLOR, 0);
	printColor("> $ ", TERMINAL_BLUE, 0);

	//	createFGProcess("sleepyJoe", sleepyJoe, 0, NULL );

	/* --------------------------PIPE TEST---------------------------- */

	// writer | reader
	// int pipeFD = openPipe("superpipe");
	// print("PIPE opened FD = %d\n", pipeFD);
	// int writeFD[] = {0, pipeFD};
	// int readFD[] = {pipeFD, 1};
	// int writerPID = createProcess("writer", writer, 0, NULL, writeFD);
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
