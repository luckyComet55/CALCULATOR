#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <complex.h>
#include "strings_analysis.h"

QUEUE * input_data(FILE * fr) {
    char elem;
    QUEUE * input = conf_queue();
    elem = (char)getc(fr);
    while (elem != '\n' && elem != EOF) {
        if(isspace(elem)) {
            elem = (char) getc(fr);
            continue;
        }
        int i = 0;
        char * word = (char*) calloc(256, sizeof (char));
        for (int j = 0; j < 256; ++j) {
            word[i] = 0;
        }
        if(isdigit(elem)) {
            while(isdigit(elem) || elem == '.' || elem == 'j') {
                //printf("%c", elem);
                word[i++] = elem;
                elem = (char)getc(fr);
            }
        } else if(isalpha(elem)) {
            while(isalpha(elem) || isdigit(elem) || elem == '_') {
                word[i++] = elem;
                elem = (char) getc(fr);
            }
        } else {
            word[i] = elem;
            elem = (char)getc(fr);
        }
        add(input, word, -1);
        free(word);
    }
    return input;
}

double str_to_val(char const * string) {
    double value = 0;
    int flag_dot = 0;
    int index = 0;
    while (string[index] != 0 && string[index] != 'j') {
        if(string[index] == '.') {
            flag_dot = 1;
            index++;
            break;
        }
        value = value * 10 + string[index] - 48;
        index++;
    }
    if(flag_dot) {
        int exp = 10;
        while(string[index] != 0 && string[index] != 'j') {
            value += (double)(string[index] - 48) / exp;
            index++;
            exp *= 10;
        }
    }
    return value;
}

int unary_operations(char * func) {
    char funcDB[11][256] = {
            {"sin\0"}, {"cos\0"},
            {"tg\0"}, {"ln\0"},
            {"sqrt\0"}, {"abs\0"},
            {"exp\0"}, {"real\0"},
            {"imag\0"}, {"mag\0"},
            {"phase\0"}
    };
    for (int i = 0; i < 11; ++i) {
        //printf("Checking source %s with unary %s\n", func, funcDB[i]);
        if(strcmp(func, funcDB[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int binary_operations(char * func) {
    char funcDB[7][256] = {
            {"*\0"}, {"+\0"},
            {"^\0"},  {"-\0"},
            {"/\0"}, {"log\0"},
            {"pow\0"}
    };
    for (int i = 0; i < 7; ++i) {
        //printf("Checking source %s with binary %s\n", func, funcDB[i]);
        if(strcmp(func, funcDB[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void print_postfix(QUEUE * postfix_notation) {
    printf("Reverse Polish Notation:\n");
    QUEUE * temp = postfix_notation;
    temp = temp->next;
    while(temp != NULL) {
        printf("%s ", temp->value);
        temp = temp->next;
    }
    printf("\n");
}

void print_expr(QUEUE * expr) {
    printf("Expression:\n");
    QUEUE * temp = expr;
    temp = temp->next;
    while(temp != NULL) {
        printf("%s ", temp->value);
        temp = temp->next;
    }
    printf("\n");
}

void print_parameters(PARAMETERS * Head) {
    PARAMETERS * temp = Head;
    temp = temp->next;
    while(temp != NULL) {
        printf("Parameter '%s':\n", temp->name);
        print_postfix(temp->expr);
        temp = temp->next;
    }
}

void print_res(double complex res) {
    printf("Result:\n");
    if(cimag(res) == 0) {
        printf("%lf", creal(res));
    } else if(creal(res) == 0) {
        printf("%lfi", cimag(res));
    } else if(cimag(res) > 0) {
        printf("%lf+%lfi", creal(res), cimag(res));
    } else {
        printf("%lf%lfi", creal(res), cimag(res));
    }
}