#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "header.h"
#include "utils.h"
#include "registers.h"


FILE *createTable(FILE *input, char *outputName);
void selectFrom(FILE *from);
void searchInBinaryFile(FILE *input);

#endif