#ifndef STACK_H
#define STACK_H

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;

extern Stack waitQueue;
extern Stack inputQueue;

void initializeStack(Stack* stack);
int isStackEmpty(Stack* stack);
int isStackFull(Stack* stack);
void push(Stack* stack, int element);
int pop(Stack* stack);
int peek(Stack* stack);
void printQueue(Stack* stack);

#endif /* STACK_H */
