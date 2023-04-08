#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#define numMembers 6
#define crimeDateLen 10
#define telephoneBrandLen 12
#define bytesFixedMember 32
#define MEM_ERROR printf("MEM allocation error\n")

typedef struct data Data;

FILE *createTable(FILE *input, char *outputName);
void selectFrom(FILE *from);

#endif