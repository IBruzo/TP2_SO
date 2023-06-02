#include <shell.h>
void test_proc(int argv, char *argc[])
{
    print("TESTING PROCESSES\n");
    char *args[] = {"10"};
    test_processes(1, args);

    print("TEST ENDED\n");
}

int main()
{
    // createProcess(test_proc, 0, NULL);
    createProcess(initialize_shell, 0, NULL);

    return 0;
}
