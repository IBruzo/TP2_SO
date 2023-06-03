#include <shell.h>

void *testBGProcess(int argc, char **argv)
{
	while (1)
	{
		print("ha");
		sleep(1);
	}
}

void *initialize_shell(int argc, char **argv)
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	printColor("user@Qemu:", USER_TEXT_COLOR, 0);
	printColor("> $ ", TERMINAL_BLUE, 0);

	// bg process test
	// createBGProcess(testBGProcess, 0, NULL);

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
