#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "utils.h"
#include "funcoesFornecidas.h"

#define numMembers 6
#define crimeDateLen 10
#define telephoneBrandLen 12
#define bytesFixedMember 32

typedef struct data Data;
typedef struct search Search;

Data *readRegister(FILE *input);
int writeRegister(FILE *output, Data *tmpRegister);
Data *readBinaryRegister(FILE *input);
void printData(Data *d);
Search *createSearchArr(FILE *input, int *numberPairs);
int *search(FILE *input, Search *wanted, int numberPairs, int *sizeArrByte);

#endif