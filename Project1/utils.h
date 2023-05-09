#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define numMembers 6
#define MEM_ERROR printf("MEM allocation error\n")

char *readMember(FILE *input, char delimiter);
char *completeSetString(char *str, int lenStr);
char *completeUnsetString(char *str, int *flag);
void readCsvHeader(FILE *input);
int stringLenght(char *str);
char *superStringCopy(char *origin, int maxLen);
long long int *byteOffsetArrAppend(long long int *arr, int len, long long int byteOffset);


#endif