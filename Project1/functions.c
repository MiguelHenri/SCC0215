#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "header.h"

struct data {
    char removed;
    int crimeID;
    char *crimeDate;
    int articleNumber;
    char *telephoneBrand;
    char *crimePlace;
    char *crimeDescription;
};

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
* Function used to read one register 
*/
Data *readRegister(FILE *input) {
    // allocating memory for temporary register variable
    Data *tmpRegister = (Data *)malloc(sizeof(Data));
    if (tmpRegister == NULL) {
        MEM_ERROR;
        return NULL;
    }

    char *tmpData = NULL;
    char delimiter = ',';

    // reading register members one by one

    tmpData = readMember(input, delimiter);
    if (tmpData == NULL) return NULL;
    tmpRegister->crimeID = atoi(tmpData); // converts string into integer
    
    tmpData = readMember(input, delimiter);
    tmpRegister->crimeDate = tmpData;

    tmpData = readMember(input, delimiter);
    if (tmpData == NULL) { // dealing with missing data
        tmpRegister->articleNumber = -1;
    } 
    else {
        tmpRegister->articleNumber = atoi(tmpData); // converts string into integer
    }

    tmpData = readMember(input, delimiter);
    tmpRegister->crimePlace = tmpData;

    tmpData = readMember(input, delimiter);
    tmpRegister->crimeDescription = tmpData;

    tmpData = readMember(input, delimiter);
    tmpRegister->telephoneBrand = tmpData;

    // default = 0, will be set to 1 if the register is removed
    tmpRegister->removed = '0';

    return tmpRegister; // returns the register
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

/*
* Function that writes in a file, passed by parameter, the members of 
* a data register, also passed by parameter.
* It returns the amount of bytes written of the unset size members
*/
int writeRegister(FILE *output, Data *tmpRegister) {
    if (output == NULL || tmpRegister == NULL)
        return 0;
        
    char stringDelimiter = '|';
    char registerDelimiter = '#';

    fwrite(&(tmpRegister->removed), sizeof(char), 1, output);
    fwrite(&(tmpRegister->crimeID), sizeof(int), 1, output);

    // verifying if crimeDate needs to be completed with '$' 
    tmpRegister->crimeDate = completeSetString(tmpRegister->crimeDate, crimeDateLen);
    fwrite(tmpRegister->crimeDate, crimeDateLen, 1, output);

    fwrite(&(tmpRegister->articleNumber), sizeof(int), 1, output);
    
    // verifying if telephoneBrand needs to be completed with '$' 
    tmpRegister->telephoneBrand = completeSetString(tmpRegister->telephoneBrand, telephoneBrandLen);
    fwrite(tmpRegister->telephoneBrand, telephoneBrandLen, 1, output);

    // flag utilized to verify the need to write, or not, the '|' at the end of 
    // the data with unset size
    int writeStrDelimiter = 1;  

    // variable utilized to store the amount of bytes of the unset size data  
    int variableSize = 0;

    // verifying unset size data
    tmpRegister->crimePlace = completeUnsetString(tmpRegister->crimePlace, &writeStrDelimiter);
    fwrite(tmpRegister->crimePlace, strlen(tmpRegister->crimePlace), 1, output);
    variableSize += strlen(tmpRegister->crimePlace);
    if (writeStrDelimiter) {
        fwrite(&stringDelimiter, 1, 1, output);
        variableSize++;
    }

    // reseting the flag variable
    writeStrDelimiter = 1;

    // verifying unset size data
    tmpRegister->crimeDescription = completeUnsetString(tmpRegister->crimeDescription, &writeStrDelimiter);
    fwrite(tmpRegister->crimeDescription, strlen(tmpRegister->crimeDescription), 1, output);
    variableSize += strlen(tmpRegister->crimeDescription);
    if (writeStrDelimiter) {
        fwrite(&stringDelimiter, 1, 1, output);
        variableSize++;
    }

    fwrite(&registerDelimiter, 1, 1, output);

    return variableSize; 
}


/*
* This function creates a table (group of registers) and returns data collected from a
* .csv file 'input' in a .bin file output named 'outputName'
*/
FILE *createTable(FILE *input, char *outputName) {
    if (input == NULL || outputName == NULL) 
        return NULL;

    // opens .bin output file
    FILE *output = fopen(outputName, "wb");
    if (output == NULL) {
        printf("Opening file error\n");
        return NULL;
    }

    Header *h = createHeader(); // creates table's header
    
    // switching to inconsistent status
    updateHeaderStatus(h);
    // writes header data and sums its size to header->nextByteOffset
    addByteOffset(h, writeHeader(output, h));

    int nReg = 0; // number of registers
    Data *reg = NULL; // auxiliar register variable

    readCsvHeader(input); // discards .csv first row
    do {
        reg = readRegister(input);
        if (reg == NULL)
            break;

        addByteOffset(h, writeRegister(output, reg) + bytesFixedMember); // updates header->nextByteOffset

        add1FileReg(h); // sums 1 to header->number(of)Registers

    } while (!feof(input));

    updateHeader(output, h); 

    fclose(output);

    return output; // returns file (.bin)
}

/*
* Function utilized to read a data register from a binary file
* It returns a pointer to a data register
*/
Data *readBinaryFile(FILE *input) {
    if (input == NULL) return NULL;

    Data *tmp = (Data *)malloc(sizeof(Data));
    if (tmp == NULL) {
        MEM_ERROR;
        return NULL;
    }
    if (tmp == NULL) return NULL;

    char *strAux1 = (char *)malloc(sizeof(char) * crimeDateLen);
    char *strAux2 = (char *)malloc(sizeof(char) * crimeDateLen);
    char charAux;
    int intAux;
    char delimiter = '|';

    fread(&charAux, sizeof(char), 1, input);
    tmp->removed = charAux;

    fread(&intAux, sizeof(int), 1, input);
    tmp->crimeID = intAux;

    fread(strAux2, sizeof(char) * crimeDateLen, 1, input);
    tmp->crimeDate = strAux2;

    fread(&intAux, sizeof(int), 1, input);
    tmp->articleNumber = intAux;

    fread(strAux1, sizeof(char) * telephoneBrandLen, 1, input);
    tmp->telephoneBrand = strAux1;

    strAux1 = readMember(input, delimiter);
    tmp->crimePlace = strAux1;

    strAux1 = readMember(input, delimiter);
    tmp->crimeDescription = strAux1;

    fread(&charAux, sizeof(char), 1, input);//reading register delimiter

    return tmp;
}

/*
* This function is used to print one register data in the terminal
*/
void printData(Data *d) {
    if (d == NULL) return;
    if (d->crimeDate[0] == '\0') return; // missing data
    if (d->removed == '1') return; // data was removed (not meant to be printed)

    printf("%d, ", d->crimeID);

    if(d->crimeDate[0] == '$') { // dealing with $
        printf("NULO, ");
    }
    else {
        int i = 0;
        while (i < crimeDateLen && d->crimeDate[i] != '$')
            printf("%c", d->crimeDate[i++]);
        printf(", ");
    }
    
    if (d->articleNumber == -1) // missing data flag
        printf("NULO, ");
    else
        printf("%d, ", d->articleNumber);

    if (d->crimePlace == NULL)
        printf("NULO, ");
    else
        printf("%s, ", d->crimePlace);
        
    if (d->crimeDescription == NULL)
        printf("NULO, ");
    else
        printf("%s, ", d->crimeDescription);
        
    if(d->telephoneBrand[0] == '$') // dealing with $
        printf("NULO");
    else {
        int i = 0;
        while (i < telephoneBrandLen && d->telephoneBrand[i] != '$')
            printf("%c", d->telephoneBrand[i++]);
    }
    printf("\n");
}

/*
* This function receives a .bin file 'from'
* it reads the file and prints file data collected in the terminal
*/
void selectFrom(FILE *from) {
    if (from == NULL) return;

    Data *reg = NULL;
    int flagError;

    flagError = readHeaderBinary(from);
    if (flagError == 0)
        return;
    
    while (!feof(from)) {
        reg = readBinaryFile(from);
        printData(reg);
    }
}