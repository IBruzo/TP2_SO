#include <shell.h>

void *initialize_shell(int argc, char **argv)
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	printColor("user@Qemu:", USER_TEXT_COLOR, 0);
	printColor("> $ ", TERMINAL_BLUE, 0);

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
