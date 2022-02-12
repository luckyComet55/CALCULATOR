#include <complex.h>
#ifndef STACKLIST_H
#define STACKLIST_H

typedef struct stackList {
    char * name;
    int priority;
    int type;
    double complex value;
    struct stackList * next;
} STACK;

STACK * init_stack();
void push(STACK * head, char * name, int priority, int type);
char * pop(STACK * head);
int get_prior(STACK * head);
double complex get_value(STACK * head);
void push_value(STACK * head, double complex val);
double complex pop_value(STACK * head);
void delete_stack(STACK * head);
int check_type(STACK * head);

#endif //STACKLIST_H