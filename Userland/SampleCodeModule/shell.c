
#include <shell.h>

void initialize_shell()
{
	putSquare(0, 0, 1024, BACKGROUND_COLOR);
	/* nunca supera esta linea :((((((( */
	printColor("user@Qemu:", USER_TEXT_COLOR, 0);
	printColor("> $ ", TERMINAL_BLUE, 0);
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();
	print("jose");
	halt();

	while (1)
	{
		char c = getchar();
		checkKey(c);
	}
}
