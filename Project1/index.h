#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include <stdlib.h>

#define maxLenStr 12

typedef struct indexData IndexData;
typedef struct indexHeader IndexHeader;

IndexData *createIndexArr(FILE *input, IndexHeader *h, char *indexType, char *memberName);
void writeFileIndex(FILE *index, IndexData *arr, IndexHeader *h, char *memberName);

#endif