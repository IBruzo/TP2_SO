#include <shell.h>

int main()
{
    createFGProcess("shell", initialize_shell, 0, NULL);
    return 0;
}
