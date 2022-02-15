#include <string.h>
#include <stdlib.h>
#include <complex.h>
#include "stackList.h"

STACK * init_stack() {
    STACK * head = (STACK*) malloc(sizeof (STACK));
    head->name = (char*) calloc(16, sizeof (char));
    strcpy(head->name, "HeadNone\0");
    head->priority = -1;
    head->type = -1;
    head->next = NULL;
    return head;
}

void push(STACK * head, char * name, int priority, int type) {
    STACK * newNode = (STACK*) malloc(sizeof(STACK));
    newNode->priority = priority;
    newNode->next = head->next;
    newNode->type = type;
    newNode->name = (char*) calloc(16, sizeof(char));
    strcpy(newNode->name, name);
    head->next = newNode;
}

void remove_node(STACK * head) {
    STACK * temp = head->next;
    head->next = temp->next;
    if(temp->name == NULL) {
        exit(100);
    } else {
        free(temp->name);
    }
    free(temp);
}

char * pop(STACK * head) {
    if(head->next == NULL) {
        exit(-1);
    }
    char * temp = (char*) calloc(16, sizeof (char));
    strcpy(temp, head->next->name);
    remove_node(head);
    return temp;
}

double complex get_value(STACK * head) {
    return head->next->value;
}

void push_value(STACK * head, double complex val) {
    STACK * newNode = (STACK*) malloc(sizeof (STACK));
    newNode->next = head->next;
    newNode->value = val;
    newNode->name = (char*) malloc(sizeof (char));
    head->next = newNode;
}

double complex pop_value(STACK * head) {
    if(head->next == NULL) {
        exit(-1);
    }
    double complex val = head->next->value;
    remove_node(head);
    return val;
}

int get_prior(STACK * head) {
    if(head->next == NULL) {
        return -1;
    }
    return head->next->priority;
}

void delete_stack(STACK * head) {
    while (head->next != NULL) {
        pop(head);
    }
    free(head);
}

int check_type(STACK * head) {
    if(head->next == NULL) {
        return -1;
    }
    return head->next->type;
}