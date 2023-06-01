#ifndef SEARCH_H
#define SEARCH_H

#include <stdio.h>
#include <stdlib.h>
#include "registers.h"

typedef struct result Result;
typedef struct search Search;

Search *createSearchArr();
Result *sequentialSearch(FILE *input, Search *wanted, Header *h);
Result *createResult();
long long int *byteOffsetArrAppend(long long int *arr, int len, long long int byteOffset);
Result *verifyingRegRequirements(FILE *dataFile, Result *resArr, Search *wanted);

#endif