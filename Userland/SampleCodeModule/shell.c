#include <shell.h>

void *testBGProcess(int argc, char **argv)
{
	while (1)
	{
		print("ha");
		sleep(1);
	}
}

void *waitProcess1(int argc, char **argv)
{
	print("IN [%s]\n", argv[1]);
	sleep(5);
	print("OUT [%s]\n", argv[1]);
	exit();
}
void *waitProcess2(int argc, char **argv)
{
	print("IN [%s]\n", argv[1]);
	print("OUT [%s]\n", argv[1]);
	exit();
}
void *waitProcess3(int argc, char **argv)
{
	print("IN [%s]\n", argv[1]);
	print("OUT [%s]\n", argv[1]);
	exit();
}

void *initialize_shell(int argc, char **argv)
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	printColor("user@Qemu:", USER_TEXT_COLOR, 0);
	printColor("> $ ", TERMINAL_BLUE, 0);

	/* --------------------------BG TEST------------------------------ */

	// createBGProcess(testBGProcess, 0, NULL);

	/* --------------------------WAIT TEST---------------------------- */

	// uint64_t pids[3];
	//
	// char *argv1[] = {"10", "1", NULL};
	// char *argv2[] = {"10", "2", NULL};
	// char *argv3[] = {"10", "3", NULL};
	//
	// pids[0] = createFGProcess(waitProcess1, 2, argv1);
	// pids[1] = createFGProcess(waitProcess2, 2, argv2);
	// pids[2] = createFGProcess(waitProcess3, 2, argv3);
	// waitPid(pids[2]);
	// waitPid(pids[0]);
	// waitPid(pids[1]);

	/* ---------------------------------------------------------------- */

	while (1)
	{
		char c = getchar();
		if (c != 0)
		{
			checkKey(c);
		}
	}
	return NULL;
}
