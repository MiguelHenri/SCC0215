#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include "registers.h"

#define maxLenStr 12

typedef struct indexData IndexData;
typedef struct indexHeader IndexHeader;

IndexData *createIndexArr(FILE *input, IndexHeader *h, char *indexType, char *memberName);
void writeFileIndex(FILE *index, IndexData *arr, IndexHeader *h, char *memberName);
IndexHeader *createIndexHeader();
void setIndexHeaderStatus(IndexHeader *h, char status);
void setIndexHeaderNumReg(IndexHeader *h, int numReg);
IndexData *readFileIndex(FILE *indexFile, char *memberName, IndexHeader *header);
long long int *searchInIndexArr(IndexData *arr, IndexHeader *h, Search *wanted, int iteration, char *memberName, int *sizeArrByte);
void printIndex(IndexData *arr, IndexHeader *h);
#endif