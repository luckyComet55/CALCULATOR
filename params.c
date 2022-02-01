#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "params.h"

PARAMETERS * ParamCon(char name[15]) {
    PARAMETERS * NewPar = (PARAMETERS*) malloc(sizeof (PARAMETERS));
    NewPar->next = NULL;
    NewPar->counter = 1;
    strcpy(NewPar->name, name);
    NewPar->expr = (QUEUE*) malloc(sizeof (QUEUE));
    return NewPar;
}

PARAMETERS * MakeHead() {
    PARAMETERS * Head = (PARAMETERS*) malloc(sizeof (PARAMETERS));
    Head->counter = 0;
    Head->next = NULL;
    strcpy(Head->name, "HeadNone\0");
    return Head;
}

void DeleteList(PARAMETERS * Head) {
    PARAMETERS * temp1 = Head;
    PARAMETERS * temp2 = temp1;
    while(temp2 != NULL) {
        temp2 = temp2->next;
        free(temp1);
        temp1 = temp2;
    }
}

int FindParam(PARAMETERS * head, char name[15]) {
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