#include "waitStack.h"


WaitStack waitQueue;

void initializeWaitStack(WaitStack* stack) {
    stack->top = -1;
}

int isWaitStackEmpty(WaitStack* stack) {
    return (stack->top == -1);
}

int isWaitStackFull(WaitStack* stack) {
    return (stack->top == MAX_SIZE - 1);
}

void pushWaitStack(WaitStack* stack, int pid, int cpid) {
    if (isWaitStackFull(stack)) {
        return;
    }

    Process process;
    process.pid = pid;
    process.cpid = cpid;

    stack->data[++(stack->top)] = process;
}

Process popWaitStack(WaitStack* stack) {
    Process emptyProcess = { -1, -1 };
    if (isWaitStackEmpty(stack)) {
        return emptyProcess;
    }

    return stack->data[(stack->top)--];
}

Process peekWaitStack(WaitStack* stack) {
    Process emptyProcess = { -1, -1 };
    if (isWaitStackEmpty(stack)) {
        return emptyProcess;
    }

    return stack->data[stack->top];
}

void printWaitStack(WaitStack* stack) {
    print("WaitStack Contents:\n");

    if (isWaitStackEmpty(stack)) {
        print("Stack is empty.\n");
        return;
    }

    for (int i = stack->top; i >= 0; i--) {
        Process process = stack->data[i];
        print("PID: %d, CPID: %d\n", process.pid, process.cpid);
    }
}