#include "stack.h"
#include <stdlib.h>
/*
 * Данный файл имеет функции, реализующие
 * стек на диамическом массиве типа char.
 * Сама структура STACK объявлена
 * в одноимённом заголовочном файле .h
 */


/*
 * Конструктор (типа как в ООП) стека.
 * Выделяется память под массив, текущий
 * размер массива приравнивается к нулю,
 * ёмкость массива приравнивается к единице.
 */
STACK * StackCon() {
    STACK * thisStack = malloc(sizeof(STACK));
    thisStack->size = 0;
    thisStack->capacity = 1;
    thisStack->data = (char*) calloc(thisStack->capacity, sizeof(char));
    return thisStack;
}

/*
 * Увеличивает ёмкость массива вдвое
 * и перевыделяет память.
 */
void StackResize(STACK * oldStack) {
    oldStack->capacity *= 2;
    oldStack->data = (char*) realloc(oldStack->data, oldStack->capacity * sizeof(char));
}

/*
 * Очищает выделенную под массив
 * память и память, выделенную под структуру.
 */
void StackDel(STACK * thisStack) {
    free(thisStack->data);
    free(thisStack);
}

/*
 * Добавляет значение в массив
 * (увеличивает ёмкость, если требуется),
 * увеличивает текущий размер на единицу.
 */
void Insert(STACK * thisStack, char value) {
    if(thisStack->size + 1 > thisStack->capacity) {
        StackResize(thisStack);
    }
    thisStack->data[thisStack->size] = value;
    thisStack->size++;
}

/*
 * Уменьшает текущий размер на единицу,
 * возвращает последний элемент массива.
 */
char Erase(STACK * thisStack) {
    thisStack->size--;
    return thisStack->data[thisStack->size];
}

/*
 * Просто возвращает последний элемент массива.
 * Если массив пуст, возвращает ASCII код нуля.
 */
char Top(STACK * thisStack) {
    if(thisStack->size == 0) {
        return '0';
    }
    return thisStack->data[thisStack->size-1];
}