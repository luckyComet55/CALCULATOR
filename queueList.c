#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queueList.h"

QUEUE * conf_queue() {
    QUEUE * head = (QUEUE*) malloc(sizeof(QUEUE));
    head->type = -1;
    head->next = NULL;
    head->value = (char*) calloc(256, sizeof(char));
    for (int i = 0; i < 256; ++i) {
        head->value[i] = 0;
    }
    strcpy(head->value, "HeadNone\0");
    return head;
}

void add(QUEUE * head, char * value, int type) {
    QUEUE * temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    QUEUE * newNode = (QUEUE*) malloc(sizeof (QUEUE));
    newNode->value = (char*) calloc(256, sizeof(char));
    for (int i = 0; i < 256; ++i) {
        newNode->value[i] = 0;
    }
    strcpy(newNode->value, value);
    newNode->type = type;
    newNode->next = NULL;
    temp->next = newNode;
}

char * erase(QUEUE * head) {
    if(head->next == NULL) {
        printf("\nOops! Memory limit!\n");
        return "\0\0";
    }
    QUEUE * temp = head->next;
    char * value = (char*) calloc(256, sizeof(char));
    for (int i = 0; i < 256; ++i) {
        value[i] = 0;
    }
    strcpy(value, temp->value);
    head->next = temp->next;
    free(temp);
    return value;
}

void copy_queue(QUEUE * dest, QUEUE * source) {
    QUEUE * tempSource = source->next;
    while(tempSource != NULL) {
        add(dest, tempSource->value, tempSource->type);
        tempSource = tempSource->next;
    }
}

int get_type(QUEUE * head) {
    if(head->next == NULL) {
        return -1;
    }
    QUEUE * temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    return temp->type;
}

char * get_value(QUEUE * head) {
    if(head->next == NULL) {
        return "\0\0";
    }
    QUEUE * temp = head->next;
    /*while (temp->next != NULL) {
        temp = temp->next;
    }*/
    return temp->value;
}

void delete_queue(QUEUE * head) {
    QUEUE * temp = head;
    while(head->next != NULL) {
        erase(head);
    }
    free(head);
}