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
    int flag_dot = 0;
    int index = 0;
    for (; string[index] != 0; ++index) {
        if(string[index] == '.') {
            flag_dot = 1;
            index++;
            break;
        }
        value = value * 10 + string[index] - 48;
    }
    if(flag_dot) {
        int exp = 10;
        while(string[index] != 0) {
            value += (double)(string[index] - 48) / exp;
            index++;
            exp *= 10;
        }
    }
    printf("Translated from string to double: %s -- %lf\n", string, value);
    return value;
}

int UnaryOperations(char * func) {
    char funcDB[10][256] = {
            {"sin\0"}, {"cos\0"},
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
    printf("Args: %lf %lf\nResult: %lf\n", a, b, log(b) / log(a));
    return log(b) / log(a);
}

double NatLog(double a) {
    printf("Args: %lf\nResult: %lf\n", a, log(a));
    return log(a);
}

double Sin(double a) {
    printf("Args: %lf\nResult: %lf\n", a, sin(a));
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
    } else if(strcmp(func, "*\0") == 0 || strcmp(func, "/\0") == 0) {
        return 3;
    }
    return 5;
}

QUEUE * get_args(QUEUE * input, int mark_if_comma) {
    QUEUE * argsQueue = conf_queue();
    int bracketCounter = 1;
    char * word = (char*) calloc(256, sizeof (char));
    for (int i = 0; i < 256; ++i) {
        word[i] = 0;
    }
    strcpy(word, get_value(input));
    while(bracketCounter > 0){
        if (bracketCounter == 1 && (strcmp(get_value(input), mark_if_comma ? ",\0" : ")\0") == 0)) {
            bracketCounter--;
            free(erase(input));
            continue;
        }
        bracketCounter += (strcmp(get_value(input), "(\0") == 0);
        bracketCounter -= (strcmp(get_value(input), ")\0") == 0);
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
        int type = get_type(input);
        strcpy(word, erase(input));
        printf("\nWord: %s\nType: %d\n", word, type);
        switch (type) {
            case operand:
                printf("Operand: %s\n", word);
                ans[top] = str_to_val(word);
                top++;
                break;
            case variable:
                printf("Parameter '%s'!\n", word);
                PARAMETERS * temp = ReturnParam(paramList, word);
                QUEUE * tempExpr = conf_queue();
                copy_queue(tempExpr, temp->expr);
                ans[top] = postfix_to_ans(temp->expr, paramList);
                temp->expr = tempExpr;
                top++;
                break;
            case binary:
                printf("Binary!\n");
                ans[top - 2] = binFuncs[BinaryOperations(word)](ans[top - 2], ans[top - 1]);
                top--;
                break;
            case unary:
                printf("Unary!\n");
                ans[top - 1] = unFuncs[UnaryOperations(word)](ans[top - 1]);
                break;
            default:
                printf("Something is wrong!\n");
                break;
        }
        /*if(isdigit(word[0])) {
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
        }*/
        if(top == capacity) {
            ans = (double*) realloc(ans, capacity * 2 * sizeof(double ));
            capacity *= 2;
        }
        free(word);
    }
    delete_queue(input);
    return ans[0];
}