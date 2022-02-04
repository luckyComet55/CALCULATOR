#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "queue.h"
#include "params.h"
#include "stack.h"
#include "base_arithmetics.h"


void ParamCon(PARAMETERS * Head, char name[15]) {
    PARAMETERS * temp = Head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    PARAMETERS * NewPar = (PARAMETERS*) malloc(sizeof (PARAMETERS));
    NewPar->next = NULL;
    NewPar->counter = 1;
    strcpy(NewPar->name, name);
    NewPar->val = 0; // ??????????????????
    temp->next = NewPar;
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

PARAMETERS * FindParam(PARAMETERS * head, char name[15]) {
    PARAMETERS * temp = head;
    while(temp != NULL) {
        if(strcmp(name, temp->name) == 0) {
            temp->counter++;
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void ReadParam(PARAMETERS * Head, QUEUE * input){
    QUEUE * polNot = QueueCon();
    STACK * tempStack = StackCon();
    char name[15] = {0};
    char symb = Pop(input);
    while (isalpha(symb)){    // Считывает имя вводимой переменной
        int i = 0;
        while (isalpha(symb)) {
            name[i++] = symb;
            if (isalpha(Peek(input))) {
                symb = Pop(input);
            } else {
                symb = Peek(input);
            }
        }
    }
    PARAMETERS * temp = FindParam(Head, name);
    if (temp != NULL) {
        while (input->size - input->firstPos > 0) { // Считает значение переменной
            MainProcessing(input, tempStack, polNot, Head);
            temp->val = RearrangementOutput(polNot, Head);
        }
    }

    //// Debug output. Delete in release version
    printf("\nParameter %s = %f\n", name, temp->val);
    ////

    QueueDel(polNot);
    StackDel(tempStack);
}

