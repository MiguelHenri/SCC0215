#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define delimitadorString |
#define delimitadorStruct #
#define numCampos 6
#define nulo "$$$"
#define lenDataCrime 10
#define lenMarcaCelular 12



typedef struct data Data;

char *readMember(FILE *input);
FILE *createTable(FILE *input);
void writeRegister(FILE *output, Data *tmpRegister);


#endif