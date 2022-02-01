#include <stdio.h>
#include <ctype.h>
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
    while (elem != '\n') {
        Push(input, (char) elem);
        elem = (char) getc(fr);
    }
    return input;
}

int main() {
    QUEUE * input;
    input = InputData(stdin);
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
    //// Debug output. Delete in release version
    printf("\nReverse Polish Notation:\n");
    for (int i = polNot->firstPos; i < polNot->size; ++i) {
        if(isdigit(polNot->data[i]) && isdigit(polNot->data[i + 1]))
            printf("%c", polNot->data[i]);
        else if((int)polNot->data[i] != '!')
            printf("%c ", polNot->data[i]);
    }
    ////
    printf("\nResult:\n");
    printf("%lf", RearrangementOutput(polNot));
    QueueDel(polNot);
    QueueDel(input);
    DeleteList(paramList);
    StackDel(tempStack);
    return 0;
}
