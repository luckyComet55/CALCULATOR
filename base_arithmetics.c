#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "queue.h"
#include "stack.h"
#include "params.h"
#include "base_arithmetics.h"


/*
 * В данный файл вынесены функции основной обработки
 * арифметических операций (что ясно по названию). На
 * данном этапе разработки я вижу его использование так -
 * - при большом количестве заданных выражений придётся
 * создать связанный список (очередь?) объектов, каждый
 * элемент которого являет собой операнд в выражении
 * (не слишком ли много лишней памяти? возможно лучше
 * будет сделать стек только для параметров, скорее
 * даже так). При нахождении параметра будет вызываться
 * функция поиска его в списке/очереди. Так как параметр
 * в любом случае задан каким-либо выражением, то для его
 * вычисления будут запускаться функции MainProcessing
 * и RearrangementOutput, являющие собой построение
 * постфиксной записи и последующий подсчёт результата.
 * Аналогично программа будет работать при нахождении
 * именованного параметра внутри каждого под-выражения,
 * таким образом, программа рабоает рекурсивно.
 * Далее последует описание использованных переменных:
 *
 * input - очередь входных данных. В неё последовательно записаны
 * все символы, считанные из файла.
 *
 * tempStack - стек, в который, согласно алгоритму Дейкстры,
 * заносятся операторы (и выгружаются в очередь polNot при нахождении
 * операторов высшего приоритета). В дальнейшем также будет принимать
 * в себя функции.
 *
 * polNot - очередь, в заполненном варианте представляющая собой
 * постфиксную форму изначального арифметического (алгебраического)
 * выражения.
 *
 * localShift - локальный сдвиг относительно начала выражения.
 *      В данном случае "начало выражения" - начало каждой
 *      подстроки (включая главную), выделенную скобками (в
 *      случае с главной подстрокой - САМОЕ начало строки).
 *      Данная подстрока нужна для обнаружения унарных минусов
 *      и их последующей обработки. Приравнивается к -1 при нахождении
 *      открывающей скобки. После каждого шага инкрементируется.
 *
 * symb - переменная, хранящая в себе каждый новый считанный из
 *      очереди input символ.
 *
 * temp - переменная, нужная только для обработки закрывающей
 *      скобки. При нахождении оной из стека операторов tempStack
 *      выгружаются все операторы, стоящие после открывающей скобки.
 *      Сама открывающая скобка выбрасывается из стека, но, как и
 *      скобка закрывающая, не попадает в очередь.
 *
 * prior - массив, в котором хранятся приоритеты операторов. Суть
 *      хранения такова - каждый индекс есть последняя цифра ASCII
 *      кода каждого оператора. В будущем (при добавления функций
 *      в кхм функционал калькулятора) придётся подредактировать
 *      это. Хотелось бы придумать что-то элегантное, не заставляющее
 *      переписать вообще всё.
 *
 * ans - динамический массив типа double, хранящий в себе промежуточные
 *      результаты, а также сам ответ (по результатам работы функции
 *      RearrangementOutput будет находиться в ячейке с нулевым сдвигом).
 *      Первоначально размера 1, будет увеличиваться со временем выполнения
 *      программы. Возможно, не самая лучшая идея задать ему единичный
 *      размер в начале.
 *
 * top - индекс, указывающий на позицию следующего операнда (числа!)
 *      в массиве ans. Следующего - значит, последний добавленный
 *      операнд лежит в ячейке с индексом top - 1.
 *
 * capacity - величина, указывающая текущую вместимость массива ans.
 *      Также, при достижении значения top значения capacity, на
 *      массив ans должна быть перевыделена память, вдвое превышающая
 *      текущее значение capacity. После этого значение capacity также
 *      увеличивается вдвое.
 *
 * operand - переменная типа double, хранящая в себе выгруженный из
 *      очереди polNot операнд (число!). Каждый операнд заносится в
 *      массив ans.
 * */

void MainProcessing(QUEUE * input, STACK * tempStack, QUEUE * polNot, PARAMETERS * paramList) {
    int localShift = 0;
    while(input->size - input->firstPos > 0) { // Цикл выполняется до тех пор, пока
                                               // позиция первого элемента в очереди
                                               // не будет совпадать с размером очереди.
                                               // При этом стоить помнить, что размер очереди
                                               // определяется количеством элементов в нём.
        char symb = Pop(input);
        int prior[10] = {0};
        prior[0] = 1; // '('
        prior[1] = 1; // ')'
        prior[2] = 3; // '*'
        prior[3] = 2; // '+'
        prior[4] = 4; // '^'
        prior[5] = 2; // '-'
        prior[7] = 3; // '/'
        char temp;
        if(isdigit(symb)) {                             // Если встреченный символ является цифрой,
            while(isdigit(symb)) {                      // то он добавляется в очередь polNot, как и
                Push(polNot, symb);        // все последующие за ним символы, являющиеся цифрами.
                if(isdigit(Peek(input))) {   // Так происходит добавление чисел целиком.
                    symb = Pop(input);          // тем временем сама очередь input постепенно очищается.
                } else {
                    symb = Peek(input);
                }
            }
            Push(polNot, '!'); // Знак "!" при хранении операндов
                                               // в формате char в очереди является
                                               // знаком-разделителем (от этого
                                               // придётся избавиться, если в тз
                                               // внезапно добавят факториал).
        } else if(isalpha(symb)) {
            char name[15] = { 0 };
            int i = 0;
            while(isalpha(symb)) {
                Push(polNot, symb);
                name[i++] = symb;
                if(isalpha(Peek(input))) {
                    symb = Pop(input);
                } else {
                    symb = Peek(input);
                }
            }
            if(!FindParam(paramList, name)) {
                paramList->next = ParamCon(name);
            }
            Push(polNot, '!');
        } else if(symb != ' ') {
            switch (symb) {
                case '(':
                    /*
                     * Каждая открывающая скобка начинает новое подвыражение,
                     * вместе с чем значению localShift приравнивается значение -1.
                     * Так как после каждого повторения цикла localShift инкрементируется,
                     * то к началу следующего он будет равен нулю. Скобка заносится в стек операторов.
                     */
                    Insert(tempStack, symb);
                    localShift = -1;
                    break;
                case ')':
                    /*
                     * Как было сказано при описании переменной temp,
                     * все операторы, стоящие после открывющей скобки,
                     * выгружаются в очередь префиксной записи.
                     */
                    temp = Erase(tempStack);
                    while(temp != '(') {
                        Push(polNot, temp);
                        temp = Erase(tempStack);
                    }
                    break;
                /*
                 * Далее следуют одинаковые (за исключением минуса)
                 * действия при найденных операторах. Суть такова:
                 * В очередь постфиксной записи выгружаюся все операторы,
                 * имеющие приоритет ниже, либо такой же, как у рассматриваемого.
                 * Это делается до первого встреченного оператора с приоритетом
                 * выше такового у рассматриваемого. Этим занимается условный цикл
                 * while (действие не будет выполнено ни разу, если первый же встреченный
                 * в стеке оператор имеет преоритет выше). Затем рассматриваемый оператор заносится
                 * в стек tempStack.
                 */
                case '*':
                    while(prior[symb % 10] <= prior[Top(tempStack) % 10]) {
                        Push(polNot, Erase((tempStack)));
                    }
                    Insert(tempStack, symb);
                    break;
                case '+':
                    while(prior[(int) symb % 10] <= prior[(int) Top(tempStack) % 10]) {
                        Push(polNot, Erase(tempStack));
                    }
                    Insert(tempStack, symb);
                    break;
                case '-':
                    /*
                     * Единственная особенность обработки минуса
                     * состоит в том, что требуется находить случаи
                     * возникновения унарного минуса. Если минус - первый
                     * символ в подвырожении, значит, он унарный. Если это
                     * так, то в очередь операндов заносится нуль (и
                     * знак-разделитель после него), после чего
                     * происходит прерывание.
                     */
                    if (localShift == 0) {
                        Push(polNot, '0');
                        Push(polNot, '!');
                        Insert(tempStack, symb);
                        break;
                    }
                    while(prior[(int) symb % 10] <= prior[(int) Top(tempStack) % 10]) {
                        Push(polNot, Erase(tempStack));
                    }
                    Insert(tempStack, symb);
                    break;
                case '/':
                    while(prior[symb % 10] <= prior[Top(tempStack) % 10]) {
                        Push(polNot, Erase((tempStack)));
                    }
                    Insert(tempStack, symb);
                    break;
                case '^':
                    while(prior[symb % 10] <= prior[Top(tempStack) % 10]) {
                        Push(polNot, Erase((tempStack)));
                    }
                    Insert(tempStack, symb);
                    break;
                default:
                    break;
            }
        }
        localShift++;
    }

    // Все оставшиеся операнды выгружаются из стека tempStack в очередь polNot
    while(tempStack->size > 0) {
        Push(polNot, Erase(tempStack));
    }
}

double RearrangementOutput(QUEUE * polNot) {
    double * ans = (double*) calloc(1, sizeof(double));
    int top = 0, capacity = 1;
    double operand;
    while(polNot->size - polNot->firstPos > 0) { // Цикл выполняется до тех пор, пока
                                                 // позиция первого элемента в очереди
                                                 // не будет совпадать с размером очереди.
                                                 // При этом стоить помнить, что размер очереди
                                                 // определяется количеством элементов в нём.
        operand = 0;
        if(isdigit(Peek(polNot))) {
            /*
             * Если первый встреченный элемент - цифра,
             * то начинается преобразование строки в
             * вещественное число.
             */
            while(isdigit(Peek(polNot))) {
                operand = operand * 10 + Pop(polNot) - 48;
            }
            /*
             * В массиве ans в элемент под индексом top
             * записывается значение найденного операнда.
             * После этого top инкрементируется. Если top
             * теперь равен capacity, то под массив ans
             * перевыделяется в два раза больше памяти,
             * вместе с чем вдвое увеличивается и сама
             * переменная capacity.
             */
            ans[top] = operand;
            top++;
            if(top == capacity) {
                ans = (double*) realloc(ans, capacity * 2 * sizeof(double ));
                capacity *= 2;
            }
        } else {
            /*
             * Далее происходит идентичные обработки
             * найденных операторов.
             */
            switch (Pop(polNot)) {
                case '*':
                    ans[top - 2] *= ans[top - 1];
                    top--;
                    break;
                case '+':
                    ans[top - 2] += ans[top - 1];
                    top--;
                    break;
                case '-':
                    ans[top - 2] -= ans[top - 1];
                    top--;
                    break;
                case '/':
                    ans[top - 2] /= ans[top - 1];
                    top--;
                    break;
                case '^':
                    ans[top - 2] = pow(ans[top - 2], ans[top - 1]);
                    top--;
                    break;
            }
        }
    }

    // В нулевой ячейке лежит результат вычислений.
    return ans[0];
}