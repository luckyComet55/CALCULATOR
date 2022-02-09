#include <math.h>
#include <complex.h>
#include <string.h>
#include "functions_module.h"

int set_priority(char * func) {
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

double complex Add(double complex a, double complex b) {
    return a + b;
}

double complex Subtract(double complex a, double complex b) {
    //printf("")
    return a - b;
}

double complex Multiply(double complex a, double complex b) {
    return a * b;
}

double complex Divide(double complex a, double complex b) {
    return a / b;
}

double complex Log(double complex a, double complex b) {
    //printf("Args: %lf %lf\nResult: %lf\n", a, b, log(b) / log(a));
    return clog(b) / clog(a);
}

double complex NatLog(double complex a) {
    //printf("Args: %lf\nResult: %lf\n", a, log(a));
    return clog(a);
}

double complex Sin(double complex a) {
    //printf("Args: \nResult: %lf\n", a, sin(a));
    return csin(a);
}

double complex Cos(double complex a) {
    return ccos(a);
}

double complex Tan(double complex a) {
    return ctan(a);
}

double complex Sqrt(double complex a) {
    return csqrt(a);
}

double complex Power(double complex a, double complex b) {
    return cpow(a, b);
}

double complex Abs(double complex a) {
    double complex b = cabs(a);
    return b;
}

double complex Exp(double complex a) {
    return cpow(M_E, a);
}

double complex Real(double complex a) {
    double complex b = creal(a);
    return b;
}

double complex Imag(double complex a) {
    double complex b = cimag(a);
    return b;
}

double complex Phase(double complex a) {
    double complex b = carg(a);
    return b;
}