#ifndef PARAMS_H
#define PARAMS_H

typedef struct parameter {
    char name[15];          // Имя параметра. Максимальная длина - 15 символов.
    QUEUE * expr;           // Очередь, содержащее выражение, которым задан параметр.
    int counter;            // Количество появлений в выражении.
    struct parameter * next;
} PARAMETERS;

void ParamCon(PARAMETERS * Head, char name[15]);
PARAMETERS * MakeHead();
void DeleteList(PARAMETERS * Head);
int FindParam(PARAMETERS * head, char name[15]);

#endif //PARAMS_H
