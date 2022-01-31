#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue {
    char * data;
    int firstPos;
    int size;
    int capacity;
} QUEUE;

QUEUE * QueueCon();
void QueueDel(QUEUE * thisQueue);
void QueueResize(QUEUE * oldQueue);
void Push(QUEUE * thisQueue, char value);
char Pop(QUEUE * thisQueue);
char Peek(QUEUE * thisQueue);

#endif //QUEUE_H
