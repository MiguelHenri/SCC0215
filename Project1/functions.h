#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "header.h"
#include "utils.h"
#include "registers.h"
#include "index.h"


FILE *createTable(FILE *input, char *outputName);
void selectFrom(FILE *from);
void createIndexFile(FILE *input, char *memberName, char *indexType, char *nameIndexFile);
void searchInBinaryFile(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberSearches);
void insertRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberInsert);

#endif