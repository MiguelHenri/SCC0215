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
void deleteRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberDeletions);
void insertRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberInsert);
void updateRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numUpdates);

#endif