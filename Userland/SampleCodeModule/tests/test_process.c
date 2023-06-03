#include "tests.h"
#include "library.h"

enum State
{
    RUNNING,
    BLOCKED,
    KILLED
};

typedef struct P_rq
{
    int32_t pid;
    enum State state;
} p_rq;

void printTableHeader()
{
    print("===============================================================\n");
    print("| id              | action               |state               |\n");
    print("===============================================================\n");
}

// Function to print a row in the table
void printTableRow(int pid, const char *action, const char *state)
{
    print("|%d              | %s              |%s             |\n", pid, action, state);
}

int64_t test_processes(uint64_t argc, char *argv[])
{
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {0};

    if (argc != 1)
    {
        return -1;
    }
    if ((max_processes = satoi(argv[0])) <= 0)
    {
        return -1;
    }

    p_rq p_rqs[max_processes];

    while (1)
    {
        printTableHeader();
        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++)
        {
            p_rqs[rq].pid = createFGProcess("loopPr", endless_loop_print, 0, argvAux);

            if (p_rqs[rq].pid == -1)
            {
                print("test_processes: ERROR creating process %d\n", p_rqs[rq].pid);
                return -1;
            }
            else
            {
                p_rqs[rq].state = RUNNING;
                alive++;
            }

            // Print table entry for the pushed process
            printTableRow(p_rqs[rq].pid, "Creating", "Running");
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0)
        {
            for (rq = 0; rq < max_processes; rq++)
            {
                action = GetUniform(100) % 2;
                switch (action)
                {
                case 0:
                    if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
                    {
                        if (kill(p_rqs[rq].pid) == -1)
                        {
                            print("test_processes: ERROR killing process %d\n", p_rqs[rq].pid);
                            return -1;
                        }
                        p_rqs[rq].state = KILLED;
                        alive--;

                        // Print table entry for the killed process
                        printTableRow(p_rqs[rq].pid, "Killing ", "Killed ");
                    }
                    break;
                case 1:
                    if (p_rqs[rq].state == RUNNING)
                    {
                        if (block(p_rqs[rq].pid) == -1)
                        {
                            print("test_processes: ERROR blocking process %d\n", p_rqs[rq].pid);
                            return -1;
                        }
                        p_rqs[rq].state = BLOCKED;

                        // Print table entry for the blocked process
                        printTableRow(p_rqs[rq].pid, "Blocking", "Blocked");
                    }
                    break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
            {
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
                {
                    if (unblock(p_rqs[rq].pid) == -1)
                    {
                        print("test_processes: ERROR unblocking process: %d\n", p_rqs[rq].pid);
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;

                    // Print table entry for the unblocked process
                    printTableRow(p_rqs[rq].pid, "Unblock ", "Running");
                }
            }
        }
        clearScreen();
    }
}