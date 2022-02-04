#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "queue.h"
#include "stack.h"
#include "params.h"
#include "base_arithmetics.h"

/*
 * Функция-обработчик входных
 * данных из файла. Возвращает адрес
 * на очередь с входными данными.
 */
QUEUE * InputData(FILE * fr) {
    char elem;
    QUEUE * input = QueueCon();
    elem = (char)getc(fr);
    while (elem != '\n' && elem != EOF) {
        Push(input, (char) elem);
        elem = (char) getc(fr);
    }
    return input;
}

void PrintPolish(QUEUE * polNot) {
    printf("\nReverse Polish Notation:\n");
    for (int i = polNot->firstPos; i < polNot->size; ++i) {
        if(isdigit(polNot->data[i]) && isdigit(polNot->data[i + 1]))
            printf("%c", polNot->data[i]);
        else if((int)polNot->data[i] != '!')
            printf("%c ", polNot->data[i]);
    }
}

int DefineParam(PARAMETERS * Head, FILE * fr) {
    char name[15] = { 0 };
    int i = 0;
    char elem;
    elem = (char)getc(fr);

    if(elem == EOF) {
        return 0;
    }
    while(elem != '=') {
        if(elem != ' ') {
            name[i++] = elem;
        }
        elem = (char)getc(fr);
    }
    PARAMETERS * temp = Head;
    if(FindParam(Head, name)) {
        while (strcmp(temp->name, name) != 0) {
            printf("%s\n", temp->name);
            temp = temp->next;
        }
    } else {
        ParamCon(Head, name);
        while (strcmp(temp->name, name) != 0) {
            printf("%s\n", temp->name);
            temp = temp->next;
        }
    }
    printf("%s\n", temp->name);
    STACK * tempStack = StackCon();
    QUEUE * polNot = QueueCon();
    QUEUE * input = InputData(fr);
    MainProcessing(input, tempStack, polNot, Head);
    temp->expr = polNot;
    QueueDel(input);
    StackDel(tempStack);
    return 1;
}

void FillParams(PARAMETERS * Head, FILE * fr) {
    int a;
    do {
        a = DefineParam(Head, fr);
    } while (a);
}

void PrintParams(PARAMETERS * Head) {
    PARAMETERS * temp = Head;
    temp = temp->next;
    while(temp != NULL) {
        printf("Parameter '%s':", temp->name);
        PrintPolish(temp->expr);
        temp = temp->next;
    }
}

int main() {
    FILE * fr = fopen("input", "rt");
    QUEUE * input;
    input = InputData(fr);
    //// Debug output. Delete in release version
    printf("Expression:\n");
    for (int i = 0; i < input->size; ++i) {
        printf("%c", input->data[i]);
    }
    ////
    QUEUE * polNot = QueueCon();
    STACK * tempStack = StackCon();
    PARAMETERS * paramList = MakeHead();
    MainProcessing(input, tempStack, polNot, paramList);
    FillParams(paramList, fr);
    //// Debug output. Delete in release version
    PrintParams(paramList);
    PrintPolish(polNot);
    ////
    printf("\nResult:\n");
    printf("%lf", RearrangementOutput(polNot, paramList));

    QueueDel(polNot);
    QueueDel(input);
    DeleteList(paramList);
    StackDel(tempStack);
    return 0;
}
