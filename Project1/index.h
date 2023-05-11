#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include "registers.h"

#define maxLenStr 12

typedef struct indexData IndexData;
typedef struct indexHeader IndexHeader;
typedef struct result Result;
typedef struct search Search;

IndexData *createIndexArr(FILE *input, IndexHeader *h, char *indexType, char *memberName);
void writeFileIndex(FILE *index, IndexData *arr, IndexHeader *h, char *memberName);
IndexHeader *createIndexHeader();
void setIndexHeaderStatus(IndexHeader *h, char status);
void setIndexHeaderNumReg(IndexHeader *h, int numReg);
IndexData *readFileIndex(FILE *indexFile, char *memberName, IndexHeader *header);
Result *searchInIndexArr(IndexData *arr, IndexHeader *h, Search *wanted, int iteration, char *memberName);
void printIndex(IndexData *arr, IndexHeader *h);
void readIndexHeader(FILE *indexFile, IndexHeader *indexHeader);
Result *verifyingRegRequirements(FILE *input, Result *indexResArray, Search *wanted, int numRequirements);
int isMemberInIndex(Search *wanted, int iteration, char *memberNameIndex);
Result *superSearch(FILE *input, char *memberName, IndexData *arrIndex, IndexHeader *header, Search *s, int pairs);
int getResLenght(Result *r);
long long int getResByteOffset(Result *r, int pos);
Search *createSearchArr(FILE *input, int *numberPairs);
IndexData *appendIndexArray(IndexData *arr, IndexHeader *h, char *memberName, int intKey, char *strKey, long long int byteOff);
void superDelete(FILE *input, FILE *index, Result *toDelete, IndexData *indexDataArr, IndexHeader *indexHd, char *memberName);
void sortIndexArrString(IndexData *arr, int len);
void sortIndexArrInt(IndexData *arr, int len);
int getIndexArrLen(IndexHeader *h);
void printIndexHeader(IndexHeader *h);
// IndexData *indexArrayAppend(IndexData *arr, int *len, int intKey, char *strKey, long long int byteoffset);

#endif