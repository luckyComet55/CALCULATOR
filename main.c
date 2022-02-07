#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "queueList.h"
#include "params.h"
#include "base_arithmetics.h"

/*
 * Функция-обработчик входных
 * данных из файла. Возвращает адрес
 * на очередь с входными данными.
 */
QUEUE * InputData(FILE * fr) {
    char elem;
    QUEUE * input = conf_queue();
    elem = (char)getc(fr);
    while (elem != '\n' && elem != EOF) {
        if(isspace(elem)) {
            elem = (char) getc(fr);
            continue;
        }
        int i = 0;
        char * word = (char*) calloc(256, sizeof (char));
        for (int j = 0; j < 256; ++j) {
            word[i] = 0;
        }
        if(isdigit(elem)) {
            while(isdigit(elem)) {
                word[i++] = elem;
                elem = (char)getc(fr);
            }
        } else if(isalpha(elem)) {
            while(isalpha(elem)) {
                word[i++] = elem;
                elem = (char) getc(fr);
            }
        } else {
            word[i] = elem;
            elem = (char)getc(fr);
        }
        add(input, word, -1);
        free(word);
    }
    return input;
}

void PrintPolish(QUEUE * postfix_notation) {
    printf("Reverse Polish Notation:\n");
    QUEUE * temp = postfix_notation;
    temp = temp->next;
    while(temp != NULL) {
        printf("%s ", temp->value);
        temp = temp->next;
    }
    printf("\n");
}

void PrintExpression(QUEUE * expr) {
    printf("Expression:\n");
    QUEUE * temp = expr;
    temp = temp->next;
    while(temp != NULL) {
        printf("%s ", temp->value);
        temp = temp->next;
    }
    printf("\n");
}

int DefineParam(PARAMETERS * Head, FILE * fr) {
    char * name = (char*) calloc(256, sizeof (char));
    for (int i = 0; i < 256; ++i) {
        name[i] = 0;
    }
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
    if(!FindParam(Head, name)) {
        ParamCon(Head, name);
    }
    while(strcmp(temp->name, name) != 0) {
        temp = temp->next;
    }
    printf("%s\n", temp->name);
    QUEUE * input = InputData(fr);
    QUEUE * output = conf_queue();
    inf_to_postfix(input, output, Head);
    temp->expr = output;
    free(name);
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
        printf("Parameter '%s':\n", temp->name);
        PrintPolish(temp->expr);
        temp = temp->next;
    }
}

int main() {
    FILE * fr = fopen("input", "rt");
    QUEUE * input = InputData(fr);
    PrintExpression(input);
    QUEUE * postfix_not = conf_queue();
    PARAMETERS * params = MakeHead();
    inf_to_postfix(input, postfix_not, params);
    FillParams(params, fr);
    //PrintParams(params);
    PrintPolish(postfix_not);
    printf("Result:\n%lf", postfix_to_ans(postfix_not, params));
    DeleteList(params);
    return 0;
}
