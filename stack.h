#ifndef STACK_H
#define STACK_H

typedef struct stack{
    char * data;
    int size;
    int capacity;
} STACK;

STACK * StackCon();
void StackResize(STACK * oldStack);
void StackDel(STACK * thisStack);
void Insert(STACK * thisStack, char value);
char Erase(STACK * thisStack);
char Top(STACK * thisStack);

#endif //STACK_H
