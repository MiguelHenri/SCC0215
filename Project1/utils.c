#include "utils.h"

/*
* Function used to read one register member
*/
char *readMember(FILE *input, char delimiter) {

    // static variable used to keep record of the last char read in the file
    // even when the function has finished it will be useful to treat exceptions cases
    static char previousChar = 'a';

    int size = 25; // initial string (member) size
    int n_char = 0; // counter
    int flagMissingData = 0;
    char *str = (char *)malloc(sizeof(char) * size);
    if(str == NULL) {
        MEM_ERROR;
        return NULL;
    }

    char aux = fgetc(input);
    str[n_char++] = aux;

    while (!feof(input)) {
    
        // allocating more space if necessary 
        if(n_char == size) {
            size *= 2;
            str = realloc(str, sizeof(char) * size);
        }

        // verifying register's members
        if (aux == delimiter || aux == EOF || aux == '\n') {
            if (previousChar == ',') { // data missing
                flagMissingData = 1;
            }
            previousChar = aux;
            str[--n_char] = '\0';
            break;
        }

        // reading one character and storing previous one
        previousChar = aux;
        aux = fgetc(input);

        str[n_char++] = aux;
    }

    // in this case, there is data missing, returns null
    if (flagMissingData || str[0] == '\0') 
        return NULL;
    else    // returns the member 
        return str;
}
/*
* Function employed to complete strings, that have fixed lenght, with '$'
* It returns a pointer to the new string
*/
char *completeSetString(char *str, int lenStr) {
    if (str == NULL) 
        str = (char *)malloc(sizeof(char) * lenStr);
        if (str == NULL) {
            MEM_ERROR;
            return NULL;
        }
    else 
        str = realloc(str, lenStr);

    for (int i = strlen(str); i < lenStr; i++) 
        str[i] = '$';
    

    return str;
}

/*
* Function utilized to either replace a null string with a "|" or
* do nothing in case its not null
* It returns a pointer to "|" or to the same string
*/
char *completeUnsetString(char *str, int *flag) {
    if (str != NULL) return str;

    str = (char *)malloc(sizeof(char));
    if (str == NULL) {
        MEM_ERROR;
        return NULL;
    }

    str = "|";
    *flag = 0;

    return str;
}

/*
* Reading and discarding the first row of the csv file
*/
void readCsvHeader(FILE *input) {
    char *tmpData = NULL;
    char delimiter = ',';

    for (int i = 0; i < numMembers; i++) {
        tmpData = readMember(input, delimiter);
        free(tmpData);
    }
}

int stringLenght(char *str) {
    if (str == NULL) return 0;

    int count = 0;
    while (*str) {
        count++;
        *str++;
    }

    return count;
}

