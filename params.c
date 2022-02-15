#include <stdlib.h>
#include <string.h>
#include "queueList.h"
#include "params.h"

void ParamCon(PARAMETERS * Head, char * name) {
    PARAMETERS * temp = Head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    PARAMETERS * NewPar = (PARAMETERS*) malloc(sizeof (PARAMETERS));
    NewPar->next = NULL;
    NewPar->counter = 1;
    NewPar->name = (char*) calloc(16, sizeof (char));
    strcpy(NewPar->name, name);
    NewPar->expr = conf_queue();
    temp->next = NewPar;
}

PARAMETERS * MakeHead() {
    PARAMETERS * Head = (PARAMETERS*) malloc(sizeof (PARAMETERS));
    Head->counter = 0;
    Head->next = NULL;
    Head->name = (char*) calloc(16, sizeof (char));
    Head->expr = conf_queue();
    strcpy(Head->name, "HeadNone\0");
    return Head;
}

void DeleteList(PARAMETERS * Head) {
    PARAMETERS * temp1 = Head;
    PARAMETERS * temp2 = temp1;
    while(temp2 != NULL) {
        temp2 = temp2->next;
        delete_queue(temp1->expr);
        free(temp1->name);
        free(temp1);
        temp1 = temp2;
    }
}

int FindParam(PARAMETERS * head, char * name) {
    PARAMETERS * temp = head;
    while(temp != NULL) {
        if(strcmp(name, temp->name) == 0) {
            temp->counter++;
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

PARAMETERS * ReturnParam(PARAMETERS * Head, char * name) {
    PARAMETERS * temp = Head;
    while(temp != NULL) {
        if(strcmp(name, temp->name) == 0) {
            break;
        }
        temp = temp->next;
    }
    return temp;
}
