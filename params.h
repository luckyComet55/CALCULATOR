#ifndef PARAMS_H
#define PARAMS_H

typedef struct parameter {
    char name[15];          // Имя параметра. Максимальная длина - 15 символов.
    double val;             // Значение параметра.
    int counter;            // Количество появлений в выражении.
    struct parameter * next;
} PARAMETERS;

void ParamCon(PARAMETERS * Head, char name[15]);
PARAMETERS * MakeHead();
void DeleteList(PARAMETERS * Head);
PARAMETERS * FindParam(PARAMETERS * head, char name[15]);
void ReadParam(PARAMETERS * Head, QUEUE * input);

#endif //PARAMS_H
