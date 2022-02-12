#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>
#include "strings_analysis.h"
#include "params.h"
#include "stackList.h"
#include "queueList.h"
#include "base_arithmetics.h"
#include "functions_module.h"


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
    PI_Number,
    e_Number,
    imaginary
} TYPE;

QUEUE * get_args(QUEUE * input, int mark_if_comma) {
    QUEUE * argsQueue = conf_queue();
    int bracketCounter = 1;
    char * word = (char*) calloc(256, sizeof (char));
    for (int i = 0; i < 256; ++i) {
        word[i] = 0;
    }
    strcpy(word, get_string(input));
    while(bracketCounter > 0){
        if (bracketCounter == 1 && (strcmp(get_string(input), mark_if_comma ? ",\0" : ")\0") == 0)) {
            bracketCounter--;
            free(erase(input));
            continue;
        }
        bracketCounter += (strcmp(get_string(input), "(\0") == 0);
        bracketCounter -= (strcmp(get_string(input), ")\0") == 0);
        add(argsQueue, erase(input), -1);
    }
    free(word);
    return argsQueue;
}

void define_args(QUEUE * input, QUEUE * output, PARAMETERS * paramList, int isBinary, int isUnary) {
    if(isBinary) {
        inf_to_postfix(get_args(input, 1), output, paramList);
        inf_to_postfix(get_args(input, 0), output, paramList);
    }
    if(isUnary) {
        inf_to_postfix(get_args(input, 0), output, paramList);
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
            add(output, word, word[strlen(word) - 1] == 'j' ? imaginary : operand);
        } else if(isalpha(word[0])) {
            if(binary_operations(word) != -1 || unary_operations(word) != -1) {
                free(erase(input));
                define_args(input, output, paramList, binary_operations(word) != -1, unary_operations(word) != -1);
                int tempPrior = set_priority(word);
                while(tempPrior <= get_prior(operations)) {
                    int tempType = check_type(operations);
                    add(output, pop(operations), tempType);
                }
                push(operations, word, tempPrior, binary_operations(word) != -1 ? binary : unary);
            } else {
                if(strcmp(word, "PI\0") == 0) {
                    add(output, word, PI_Number);
                } else if(strcmp(word, "e\0") == 0) {
                    add(output, word, e_Number);
                } else if(strcmp(word, "j\0") == 0) {
                    add(output, "1\0", imaginary);
                } else {
                    if (!FindParam(paramList, word)) {
                        ParamCon(paramList, word);
                    }
                    add(output, word, variable);
                }
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
            int tempPrior = set_priority(word);
            while(tempPrior <= get_prior(operations)) {
                int tempType = check_type(operations);
                add(output, pop(operations), tempType);
            }
            push(operations, word, tempPrior, binary_operations(word) != -1 ? binary : unary);
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
        add(output, word, binary_operations(word) != -1 ? binary : unary);
        free(word);
    }
    delete_queue(input);
    delete_stack(operations);
}

double complex postfix_to_ans(QUEUE * input, PARAMETERS * paramList) {
    STACK * answer = init_stack();
    int top = 0, capacity = 1;
    double complex (*binFuncs[])(double complex, double complex) = {
            Multiply, Add,
            Power, Subtract,
            Divide, Log,
            Power
    };
    double complex (*unFuncs[])(double complex) = {
            Sin, Cos,
            Tan, NatLog,
            Sqrt, Abs,
            Exp, Real,
            Imag, Abs,
            Phase
    };
    while(input->next != NULL) {
        double complex args[2];
        char * word = (char*) calloc(256, sizeof(char));
        for (int i = 0; i < 256; ++i) {
            word[i] = 0;
        }
        int type = get_type(input);
        strcpy(word, erase(input));
        printf("\nWord: %s\nType: %d\n", word, type);
        switch (type) {
            case operand:
                printf("Operand: %s\n", word);
                push_value(answer, str_to_val(word));
                top++;
                break;
            case imaginary:
                printf("Imaginary number: %s\n", word);
                push_value(answer, str_to_val(word) * I);
                top++;
                break;
            case variable:
                printf("Parameter '%s'!\n", word);
                PARAMETERS * temp = ReturnParam(paramList, word);
                QUEUE * tempExpr = conf_queue();
                copy_queue(tempExpr, temp->expr);
                push_value(answer, postfix_to_ans(temp->expr, paramList));
                temp->expr = tempExpr;
                top++;
                break;
            case binary:
                printf("Binary!\n");
                args[1] = pop_value(answer);
                args[0] = pop_value(answer);
                push_value(answer, binFuncs[binary_operations(word)](args[0], args[1]));
                top--;
                break;
            case unary:
                printf("Unary!\n");
                args[1] = pop_value(answer);
                push_value(answer, unFuncs[unary_operations(word)](args[1]));
                break;
            case PI_Number:
                push_value(answer, M_PI);
                top++;
                break;
            case e_Number:
                push_value(answer, M_E);
                top++;
                break;
            default:
                printf("Something is wrong!\n");
                break;
        }
        free(word);
    }
    delete_queue(input);
    double complex res = pop_value(answer);
    delete_stack(answer);
    return res;
}