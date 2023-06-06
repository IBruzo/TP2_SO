// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>

int main()
{
    createFGProcess("shell", initialize_shell, 0, NULL);
    return 0;
}
