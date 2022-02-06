#include <string.h>
#include <stdlib.h>
#include "stackList.h"

STACK * init_stack() {
    STACK * head = (STACK*) malloc(sizeof (STACK));
    head->name = (char*) calloc(256, sizeof (char));
    for (int i = 0; i < 256; ++i) {
        head->name[i] = 0;
    }
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
    newNode->name = (char*) calloc(256, sizeof(char));
    for (int i = 0; i < 256; ++i) {
        newNode->name[i] = 0;
    }
    strcpy(newNode->name, name);
    head->next = newNode;
}

void remove_node(STACK * head) {
    STACK * temp = head->next;
    head->next = temp->next;
    free(temp->name);
    free(temp);
}

char * pop(STACK * head) {
    if(head->next == NULL) {
        return "!\0";
    }
    char * temp = (char*) calloc(256, sizeof (char));
    for (int i = 0; i < 256; ++i) {
        temp[i] = 0;
    }
    strcpy(temp, head->next->name);
    remove_node(head);
    return temp;
}

int get_prior(STACK * head) {
    if(head->next == NULL) {
        return -1;
    }
    return head->next->priority;
}

char * get_name(STACK * head) {
    if(head->next == NULL) {
        return "\0\0";
    }
    return head->next->name;
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