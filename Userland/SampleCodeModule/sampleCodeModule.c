/* sampleCodeModule.c */

#include <shell.h>

void testProcess1()
{
    while (1)
    {
        print("Chris Arrived");
    }
    return;
}

int main()
{
    /* char *args[2] = {"1", "2"}; */

    /* sys_createProcess(initializeShell) */
    initialize_shell();
    return 0;
}
