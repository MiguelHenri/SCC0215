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
long long int *search2(FILE *input, Search *wanted, int numberPairs, int *sizeArrByte);
int strMemberCompare(char *searchMember, char *searchKey , Data *reg);
char getDataRemoved(Data *d);
int getDataCrimeId(Data *d);
char *getDataCrimeDate(Data *d);
int getDataArticleNumber(Data *d);
char *getDataTelephoneBrand(Data *d);
char *getDataCrimePlace(Data *d);
char *getDataCrimeDescription(Data *d);
int regMissingData(Data *d);
int isIntegerMember(char *memberName);
int isMemberInIndex(Search *wanted, int iteration, char *memberNameIndex);
char *getSearchStrKey(Search *s, int position);
int getSearchIntKey(Search *s, int position);

#endif