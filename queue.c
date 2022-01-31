#include <stdlib.h>
#include "queue.h"
/*
 * Данный файл имеет функции, реализующие
 * очередь на диамическом массиве типа char.
 * Сама структура QUEUE объявлена
 * в одноимённом заголовочном файле .h
 */


/*
 * Конструктор (типа как в ООП) очереди.
 * Выделяется память под массив, текущий
 * размер массива приравнивается к нулю,
 * ёмкость массива приравнивается к единице,
 * позиция первого элемента приравнивается нулю.
 */
QUEUE * QueueCon() {
    QUEUE * thisQueue = malloc(sizeof(QUEUE));
    thisQueue->size = 0;
    thisQueue->capacity = 1;
    thisQueue->firstPos = 0;
    thisQueue->data = (char*) calloc(thisQueue->capacity, sizeof(char));
    return thisQueue;
}

/*
 * Очищает выделенную под массив
 * память и память, выделенную под структуру.
 */
void QueueDel(QUEUE * thisQueue) {
    free(thisQueue->data);
    free(thisQueue);
}

/*
 * Увеличивает ёмкость массива вдвое
 * и перевыделяет память.
 */
void QueueResize(QUEUE * oldQueue) {
    oldQueue->capacity *= 2;
    oldQueue->data = (char*) realloc(oldQueue->data, oldQueue->capacity * sizeof(char));
}

/*
 * Добавляет значение в массив
 * (увеличивает ёмкость, если требуется),
 * увеличивает текущий размер на единицу.
 */
void Push(QUEUE * thisQueue, char value) {
    if(thisQueue->size + 1 > thisQueue->capacity) {
        QueueResize(thisQueue);
    }
    thisQueue->data[thisQueue->size] = value;
    thisQueue->size++;
}

/*
 * Инкрементирует номер позиции первого
 * элемента, возвращает предыдущий первый элемент.
 */
char Pop(QUEUE * thisQueue) {
    thisQueue->firstPos++;
    return thisQueue->data[thisQueue->firstPos - 1];
}

/*
 * Просто возвращает первый (то есть под индексом
 * firstPos) элемент массива.
 * Если массив пуст, возвращает ASCII код нуля.
 */
char Peek(QUEUE * thisQueue) {
    if(thisQueue->size == 0) {
        return '0';
    }
    return thisQueue->data[thisQueue->firstPos];
}