#ifndef STRINGS_ANALYSIS_H
#define STRINGS_ANALYSIS_H

#include "queueList.h"
#include "params.h"
#define TOKEN_SIZE 16

QUEUE * input_data(FILE * fr);
double str_to_val(char const * string);
int unary_operations(char * func);
int binary_operations(char * func);
void print_postfix(QUEUE * postfix_notation);
void print_expr(QUEUE * expr);
void print_parameters(PARAMETERS * Head);
void print_res(double complex res);

#endif //STRINGS_ANALYSIS_H
