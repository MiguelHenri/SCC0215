#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define delimitadorString |
#define delimitadorStruct #
#define numCampos 6
#define lenDataCrime 10
#define lenMarcaCelular 12
#define bytesFixedMember 32



typedef struct data Data;

char *readMember(FILE *input, char delimiter);
FILE *createTable(FILE *input);
int writeRegister(FILE *output, Data *tmpRegister);
void selectFrom(FILE *from);


#endif