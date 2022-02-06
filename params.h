#ifndef PARAMS_H
#define PARAMS_H
#include "queueList.h"

typedef struct parameter {
    char * name;          // Имя параметра. Максимальная длина - 15 символов.
    QUEUE * expr;           // Очередь, содержащее выражение, которым задан параметр.
    int counter;            // Количество появлений в выражении.
    struct parameter * next;
} PARAMETERS;

void ParamCon(PARAMETERS * Head, char * name);
PARAMETERS * MakeHead();
void DeleteList(PARAMETERS * Head);
int FindParam(PARAMETERS * head, char * name);
PARAMETERS * ReturnParam(PARAMETERS * Head, char * name);

#endif //PARAMS_H
