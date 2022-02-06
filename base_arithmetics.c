#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "params.h"
#include "stackList.h"
#include "queueList.h"
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

typedef enum types {
    operand,
    variable,
    binary,
    unary,
} TYPE;

double str_to_val(char const * string) {
    double value = 0;
    for (int i = 0; string[i] != 0; ++i) {
        value = value * 10 + string[i] - 48;
    }
    return value;
}

int UnaryOperations(char * func) {
    char funcDB[10][256] = {
            {"sin\n"}, {"cos\0"},
            {"tg\0"}, {"ln\0"},
            {"sqrt\0"}, {"abs\0"},
            {"exp\0"}, {"imag\0"},
            {"mag\0"}, {"phase\0"}
    };
    for (int i = 0; i < 10; ++i) {
        //printf("Checking source %s with unary %s\n", func, funcDB[i]);
        if(strcmp(func, funcDB[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int BinaryOperations(char * func) {
    char funcDB[7][256] = {
            {"*\0"}, {"+\0"},
            {"^\0"},  {"-\0"},
            {"/\0"}, {"log\0"},
            {"pow\0"}
    };
    for (int i = 0; i < 7; ++i) {
        //printf("Checking source %s with unary %s\n", func, funcDB[i]);
        if(strcmp(func, funcDB[i]) == 0) {
            return i;
        }
    }
    return -1;
}

double Add(double a, double b) {
    return a + b;
}

double Subtract(double a, double b) {
    return a - b;
}

double Multiply(double a, double b) {
    return a * b;
}

double Divide(double a, double b) {
    return a / b;
}

double Log(double a, double b) {
    return log(b) / log(a);
}

double NatLog(double a) {
    return log(a);
}

double Sin(double a) {
    return sin(a);
}

double Cos(double a) {
    return cos(a);
}

double Tan(double a) {
    return tan(a);
}

double Sqrt(double a) {
    return sqrt(a);
}

double Power(double a, double b) {
    return pow(a, b);
}

double Abs(double a) {
    return a * pow(-1, (a < 0));
}

double Exp(double a) {
    return pow(M_E, a);
}

int GetPriority(char * func) {
    if(strcmp(func, "(\0") == 0 || strcmp(func, ")\0") == 0 || strcmp(func, ",\0") == 0) {
        return 1;
    } else if(strcmp(func, "^\0") == 0) {
        return 4;
    } else if(strcmp(func, "+\0") == 0 || strcmp(func, "-\0") == 0) {
        return 2;
    }
    return 3;
}

QUEUE * get_args(QUEUE * input) {
    QUEUE * argsQueue = conf_queue();
    while(strcmp(get_value(input), ",\0") != 0 && strcmp(get_value(input), ")\0") != 0) {
        add(argsQueue, erase(input), -1);
    }
    free(erase(input));
    return argsQueue;
}

void define_args(QUEUE * input, QUEUE * output, PARAMETERS * paramList, int isBinary, int isUnary) {
    char word[256] = { 0 };
    //erase(input);
    if(isBinary) {
        inf_to_postfix(get_args(input), output, paramList);
        inf_to_postfix(get_args(input), output, paramList);
    }
    if(isUnary) {
        inf_to_postfix(get_args(input), output, paramList);
    }
}

void inf_to_postfix(QUEUE * input, QUEUE * output, PARAMETERS * paramList) {

    STACK * operations = init_stack();
    int localShift = 0;
    while(input->next != NULL) {
        char * word = (char*) calloc(256, sizeof(char));
        for (int i = 0; i < 256; ++i) {
            word[i] = 0;
        }
        strcpy(word, erase(input));
        if(isdigit(word[0])) {
            add(output, word, operand);
        } else if(isalpha(word[0])) {
            if(BinaryOperations(word) != -1 || UnaryOperations(word) != -1) {
                free(erase(input));
                define_args(input, output, paramList, BinaryOperations(word) != -1, UnaryOperations(word) != -1);
                int tempPrior = GetPriority(word);
                while(tempPrior <= get_prior(operations)) {
                    int tempType = check_type(operations);
                    add(output, pop(operations), tempType);
                }
                push(operations, word, tempPrior, BinaryOperations(word) != -1 ? binary : unary);
            } else {
                if(!FindParam(paramList, word)) {
                    ParamCon(paramList, word);
                }
                add(output, word, variable);
            }
        } else {
            if(strcmp(word, "(\0") == 0) {
                localShift = -1;
                define_args(input, output, paramList, 0, 1);
                continue;
            }
            if(strcmp(word, "-\0") == 0 && localShift == 0) {
                add(output, "0\0", operand);
            }
            int tempPrior = GetPriority(word);
            while(tempPrior <= get_prior(operations)) {
                int tempType = check_type(operations);
                add(output, pop(operations), tempType);
            }
            push(operations, word, tempPrior, BinaryOperations(word) != -1 ? binary : unary);
        }
        localShift++;
        free(word);
    }
    while(operations->next != NULL) {
        char * word = (char*) calloc(256, sizeof(char));
        for (int i = 0; i < 256; ++i) {
            word[i] = 0;
        }
        strcpy(word, pop(operations));
        add(output, word, BinaryOperations(word) != -1 ? binary : unary);
        free(word);
    }
    delete_queue(input);
    delete_stack(operations);
}

double postfix_to_ans(QUEUE * input, PARAMETERS * paramList) {
    double * ans = (double*) calloc(256, sizeof(double));
    int top = 0, capacity = 1;
    double (*binFuncs[])(double, double) = {
            Multiply, Add,
            Power, Subtract,
            Divide, Log,
            Power
    };
    double (*unFuncs[])(double) = {
            Sin, Cos,
            Tan, NatLog,
            Sqrt, Abs,
            Exp
    };
    while(input->next != NULL) {
        char * word = (char*) calloc(256, sizeof(char));
        for (int i = 0; i < 256; ++i) {
            word[i] = 0;
        }
        strcpy(word, erase(input));
        if(isdigit(word[0])) {
            ans[top] = str_to_val(word);
            top++;
        } else if(isalpha(word[0])) {
            if(BinaryOperations(word) != -1) {
                printf("Binary!\n");
                ans[top - 2] = binFuncs[BinaryOperations(word)](ans[top - 2], ans[top - 1]);
                top--;
            } else if(UnaryOperations(word) != -1) {
                printf("Unary!\n");
                ans[top - 1] = unFuncs[UnaryOperations(word)](ans[top - 1]);
            } else {
                printf("Parameter '%s'!\n", word);
                PARAMETERS * temp = ReturnParam(paramList, word);
                QUEUE * tempExpr = conf_queue();
                copy_queue(tempExpr, temp->expr);
                ans[top] = postfix_to_ans(temp->expr, paramList);
                temp->expr = tempExpr;
                top++;
            }
        } else {
            ans[top - 2] = binFuncs[BinaryOperations(word)](ans[top - 2], ans[top - 1]);
            top--;
        }
        if(top == capacity) {
            ans = (double*) realloc(ans, capacity * 2 * sizeof(double ));
            capacity *= 2;
        }
        free(word);
    }
    delete_queue(input);
    return ans[0];
}

/*
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
        prior[9] = 3; // Все оставшиеся бинарные и унарные операции
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
            char opName[15] = { 0 };
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
            */
/*if(UnaryOperations(name) != -1 || BinaryOperations(name) != -1) {
                int tempSize = tempStack->size;
                if(isalpha(Top(tempStack))) {

                }
                do {
                    int j = 0;
                    if (isalpha(Top(tempStack))) {
                        while (isalpha(Top(tempStack))) {
                            opName[j++] = Erase(tempStack);
                        }
                    } else if(Top(tempStack) != '_') {
                        opName[j] = Erase(tempStack);
                    }
                } while (GetPriority(name) <= GetPriority(opName));
            }*//*

            if(!FindParam(paramList, name)) {
                ParamCon(paramList, name);
            }
            Push(polNot, '!');
        } else if(isgraph(symb)) {
            switch (symb) {
                case '(':
                    */
/*
                     * Каждая открывающая скобка начинает новое подвыражение,
                     * вместе с чем значению localShift приравнивается значение -1.
                     * Так как после каждого повторения цикла localShift инкрементируется,
                     * то к началу следующего он будет равен нулю. Скобка заносится в стек операторов.
                     *//*

                    Insert(tempStack, symb);
                    localShift = -1;
                    break;
                case ')':
                    */
/*
                     * Как было сказано при описании переменной temp,
                     * все операторы, стоящие после открывющей скобки,
                     * выгружаются в очередь префиксной записи.
                     *//*

                    temp = Erase(tempStack);
                    while(temp != '(') {
                        Push(polNot, temp);
                        temp = Erase(tempStack);
                    }
                    break;
                */
/*
                 * Далее следуют одинаковые (за исключением минуса)
                 * действия при найденных операторах. Суть такова:
                 * В очередь постфиксной записи выгружаюся все операторы,
                 * имеющие приоритет выше, либо такой же, как у рассматриваемого.
                 * Это делается до первого встреченного оператора с приоритетом
                 * ниже такового у рассматриваемого. Этим занимается условный цикл
                 * while (действие не будет выполнено ни разу, если первый же встреченный
                 * в стеке оператор имеет преоритет выше). Затем рассматриваемый оператор заносится
                 * в стек tempStack.
                 *//*

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
                    */
/*
                     * Единственная особенность обработки минуса
                     * состоит в том, что требуется находить случаи
                     * возникновения унарного минуса. Если минус - первый
                     * символ в подвырожении, значит, он унарный. Если это
                     * так, то в очередь операндов заносится нуль (и
                     * знак-разделитель после него), после чего
                     * происходит прерывание.
                     *//*

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

double RearrangementOutput(QUEUE * polNot, PARAMETERS * paramList) {
    double * ans = (double*) calloc(1, sizeof(double));
    int top = 0, capacity = 1;
    double operand;
    double (*binFuncs[])(double, double) = {
            Multiply, Add,
            Power, Subtract,
            Divide, Log,
            Power
    };
    double (*unFuncs[])(double) = {
            Sin, Cos,
            Tan, NatLog,
            Sqrt, Abs,
            Exp
    };
    while(polNot->size - polNot->firstPos > 0) { // Цикл выполняется до тех пор, пока
                                                 // позиция первого элемента в очереди
                                                 // не будет совпадать с размером очереди.
                                                 // При этом стоить помнить, что размер очереди
                                                 // определяется количеством элементов в нём.
        operand = 0;
        if(isdigit(Peek(polNot))) {
            */
/*
             * Если первый встреченный элемент - цифра,
             * то начинается преобразование строки в
             * вещественное число.
             *//*

            while(isdigit(Peek(polNot))) {
                operand = operand * 10 + Pop(polNot) - 48;
            }
            */
/*
             * В массиве ans в элемент под индексом top
             * записывается значение найденного операнда.
             * После этого top инкрементируется. Если top
             * теперь равен capacity, то под массив ans
             * перевыделяется в два раза больше памяти,
             * вместе с чем вдвое увеличивается и сама
             * переменная capacity.
             *//*

            ans[top] = operand;
            top++;
            if(top == capacity) {
                ans = (double*) realloc(ans, capacity * 2 * sizeof(double ));
                capacity *= 2;
            }
        } else if(isalpha(Peek(polNot))) {
            char name[15] = { 0 };
            int i = 0;
            while(isalpha(Peek(polNot))) {
                name[i++] = Pop(polNot);
            }
            PARAMETERS * temp = ReturnParam(paramList, name);
            int tempFirstPos = temp->expr->firstPos;
            ans[top] = RearrangementOutput(temp->expr, paramList);
            temp->expr->firstPos = tempFirstPos;
            top++;
            if(top == capacity) {
                ans = (double*) realloc(ans, capacity * 2 * sizeof(double ));
                capacity *= 2;
            }
        } else {
            */
/*
             * Далее происходит идентичные обработки
             * найденных операторов.
             *//*


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
}*/
