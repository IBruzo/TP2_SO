#include "stack.h"

Stack inputQueue;

void initStack(Stack* stack) {
    stack->top = -1;
}

int isStackEmpty(Stack* stack) {
    return (stack->top == -1);
}

int isStackFull(Stack* stack) {
    return (stack->top == MAX_SIZE - 1);
}

void push(Stack* stack, int element) {
    if (isStackFull(stack)) {
        return;
    }

    stack->data[++(stack->top)] = element;
}

int pop(Stack* stack) {
    if (isStackEmpty(stack)) {
        return -1;
    }

    return stack->data[(stack->top)--];
}

int peek(Stack* stack) {
    if (isStackEmpty(stack)) {
        return -1;
    }

    return stack->data[stack->top];
}

void printQueue(Stack* stack) {
    if (isStackEmpty(stack)) {
        print("Queue is empty.\n");
        return;
    }

    print("Queue: ");
    for (int i = stack->top; i >= 0; i--) {
        print("%d ", stack->data[i]);
    }
    print("\n");
}
