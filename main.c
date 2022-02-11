#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <string.h>
#include "strings_analysis.h"
#include "queueList.h"
#include "params.h"
#include "base_arithmetics.h"

int DefineParam(PARAMETERS * Head, FILE * fr) {
    char * name = (char*) calloc(256, sizeof (char));
    for (int i = 0; i < 256; ++i) {
        name[i] = 0;
    }
    int i = 0;
    char elem;
    elem = (char)getc(fr);

    if(elem == EOF) {
        return 0;
    }
    while(elem != '=') {
        if(elem != ' ') {
            name[i++] = elem;
        }
        elem = (char)getc(fr);
    }
    PARAMETERS * temp = Head;
    if(!FindParam(Head, name)) {
        ParamCon(Head, name);
    }
    while(strcmp(temp->name, name) != 0) {
        temp = temp->next;
    }
    printf("%s\n", temp->name);
    QUEUE * input = input_data(fr);
    QUEUE * output = conf_queue();
    inf_to_postfix(input, output, Head);
    temp->expr = output;
    free(name);
    return 1;
}

void FillParams(PARAMETERS * Head, FILE * fr) {
    int a;
    do {
        a = DefineParam(Head, fr);
    } while (a);
}

int main() {
    FILE * fr = fopen("C:\\Users\\kurik\\CLionProjects\\CALCULATOR\\input", "rt");
    QUEUE * input = input_data(fr);
    print_expr(input);
    QUEUE * postfix_not = conf_queue();
    PARAMETERS * params = MakeHead();
    inf_to_postfix(input, postfix_not, params);
    FillParams(params, fr);
    print_postfix(postfix_not);
    double complex ans = postfix_to_ans(postfix_not, params);
    print_res(ans);
    DeleteList(params);
    fclose(fr);
    return 0;
}
