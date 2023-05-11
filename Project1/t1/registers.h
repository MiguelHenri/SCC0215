#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "utils.h"
#include "funcoesFornecidas.h"
#include "index.h"

#define numMembers 6
#define crimeDateLen 10
#define telephoneBrandLen 12
#define bytesFixedMember 32

typedef struct data Data;

Data *readRegister(FILE *input);
int writeRegister(FILE *output, Data *tmpRegister);
Data *readBinaryRegister(FILE *input);
void printData(Data *d);
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
void insertRegisterInBinFile(FILE *binFile, Data *reg, Header *h);
Data *readRegisterStdin();
int intMemberCompare(char *searchMember, int searchKey, Data *reg);
int strMemberCompare(char *searchMember, char *searchKey, Data *reg);
Data *readRegisterStdin2();

#endif