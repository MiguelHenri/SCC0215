#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registers.h"

/*
    [ TO-DO ]
    fazer uma função que compara string em geral (ex: data)
    modularizar

    fazer a funcao de inserir no vetor de byteoffset

    quando vou criar o arq index, ao ler, escrever cabecelho com status inconsistente e dps escreveer consistente ?

    testar para pairs
*/

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
* Function utilized to read a data register from a binary file
* It returns a pointer to a data register
*/
Data *readBinaryRegister(FILE *input) {
    if (input == NULL) return NULL;

    Data *tmp = (Data *)malloc(sizeof(Data));
    if (tmp == NULL) {
        MEM_ERROR;
        return NULL;
    }

    char *strAux1 = (char *)malloc(sizeof(char) * crimeDateLen);
    char *strAux2 = (char *)malloc(sizeof(char) * crimeDateLen);
    char charAux;
    int intAux;
    char delimiter = '|';

    // reading the removed status
    fread(&charAux, sizeof(char), 1, input);
    tmp->removed = charAux;

    // reading the crime id
    fread(&intAux, sizeof(int), 1, input);
    tmp->crimeID = intAux;

    // reading the crime date
    fread(strAux2, sizeof(char) * crimeDateLen, 1, input);
    tmp->crimeDate = strAux2;

    // reading the article number
    fread(&intAux, sizeof(int), 1, input);
    tmp->articleNumber = intAux;

    // reading the telephone brand
    fread(strAux1, sizeof(char) * telephoneBrandLen, 1, input);
    tmp->telephoneBrand = strAux1;

    // reading the crime place
    strAux1 = readMember(input, delimiter);
    tmp->crimePlace = strAux1;

    // reading the crime description
    strAux1 = readMember(input, delimiter);
    tmp->crimeDescription = strAux1;

    fread(&charAux, sizeof(char), 1, input); //reading register delimiter '#'

    return tmp;
}

/*
* This function is used to print one register data in the terminal
*/
void printData(Data *d) {
    if (regMissingData(d) == 1) return; // missing data
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
* Auxliary function that given a string, verifies if its a name from an integer member 
* from the data struct
*/
int isIntegerMember(char *memberName) {
    char intMembers[][20] = {"idCrime", "numeroArtigo"};

    if (strncmp(memberName, intMembers[0], 7) == 0 ||
        strncmp(memberName, intMembers[1], 12) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int stringCompareWithLen(char *str1, char *str2, int len) {
    if (str1 == NULL || str2 == NULL) return -1;
    
    for (int i = 0; i < len; i++) {
        if (str1[i] != str2[i])
            return -1;
    }

    return 0;
}

/*
* Function that is used for comparing a member from the data struct and its value to
* a register that was read
*/
int intMemberCompare(char *searchMember, int searchKey, Data *reg) {
    
    // finding the member that we want to compare. Using hard coded numbers to avoid
    // getting errors because of the \0 missing. So, we compare the string until the \0
    // because we are comparing integers, we dont have to deal with null pointers
    if (strncmp(searchMember, "idCrime", 7) == 0 &&
        searchKey == reg->crimeID) {
        return 1;
    }
    else if (strncmp(searchMember, "numeroArtigo", 12) == 0 &&
        searchKey == reg->articleNumber) {
        return 1;
    }

    //returning 0 if the comparisson fails
    return 0;
}

/*
* Function that is used for comparing a member from the data struct and its value to
* a register that was read
*/
int strMemberCompare(char *searchMember, char *searchKey, Data *reg) {

    // auxiliary variable use for the strncmp, needed because of the \0 missing
    // in the end of the string
    int lenKey = strlen(searchKey);

    // finding the member that we want to compare. Using hard coded numbers to avoid
    // getting errors because of the \0 missing. So, we compare the string until the \0
    if(strncmp(searchMember, "dataCrime", 9) == 0) {

        // if the string is null, we dont use strncmp because we will get en error
        // so we assume that our key will not be null, thusfore the comparisson will
        // return 0 always
        if (reg->crimeDate == NULL) return 0;
        
        //returning the result of the comparisson of the key and register member value
        return strncmp(reg->crimeDate, searchKey, lenKey) == 0;
    }
    else if(strncmp(searchMember, "lugarCrime", 10) == 0) { 

        // same logic from above
        if (reg->crimePlace == NULL) return 0;

        //returning the result of the comparisson of the key and register member value
        return strncmp(reg->crimePlace, searchKey, lenKey) == 0;
    }
    else if(strncmp(searchMember, "marcaCelular", 12) == 0) { 

        // same logic from above
        if (reg->telephoneBrand == NULL) return 0;

        //returning the result of the comparisson of the key and register member value
        return strncmp(reg->telephoneBrand, searchKey, lenKey) == 0;
    }
    else if(strncmp(searchMember, "descricaoCrime", 14) == 0) { 
        
        // same logic from above
        if (reg->crimeDescription == NULL) return 0;

        //returning the result of the comparisson of the key and register member value
        return strncmp(reg->crimeDescription, searchKey, lenKey) == 0;
    }

    // returning 0 if theres an error
    return 0;
}

char getDataRemoved(Data *d) {
    return d->removed;
}

int getDataCrimeId(Data *d) {
    return d->crimeID;
}

char *getDataCrimeDate(Data *d) {
    return d->crimeDate;
}

int getDataArticleNumber(Data *d) {
    return d->articleNumber;
}

char *getDataTelephoneBrand(Data *d) {
    return d->telephoneBrand;
}

char *getDataCrimePlace(Data *d) {
    return d->crimePlace;
}

char *getDataCrimeDescription(Data *d) {
    return d->crimeDescription;
}

int regMissingData(Data *d) {
    return d == NULL || d->crimeDate[0] == '\0';
}

/*
* Function that inserts a register in a binary file,
* updates the header of the binary file
* and returns the byteoffset of the inserted register
*/
int insertRegisterInBinFile(FILE *binFile, Data *reg, Header *h) {
    
    //calculating the byteoffset and setting the file to the end
    long long int currentOffset = getNexByteOffset(h);
    fseek(binFile, currentOffset, SEEK_SET);

    //adding the byteoffset to the header
    int nextOffset = (writeRegister(binFile, reg) + bytesFixedMember);
    addByteOffset(h, nextOffset);

    // adding one more register in the header struct
    add1FileReg(h);

    return currentOffset;
}

/*
* Function that reads the specifications of the register that will be inserted
* it returns a pointer to a data register
*/
Data *readRegisterStdin2() {
    Data *d = (Data *)malloc(sizeof(Data));
    char isQuote;
    char quote = '"';
    int intAux;
    char *strAux = NULL;

    d->removed = '0';

    // reading the crime id, always non NULL
    fscanf(stdin, "%d ", &intAux);
    d->crimeID = intAux;

    // reading crime date
    // if it is a quote, that means we have to use quote string and its not a null value 
    isQuote = getc(stdin);
    if (isQuote == quote) { 
        strAux = (char *)malloc(sizeof(char) * 50);

        // now that the verification is over, i have to return the quote
        // ir order to use the scan_quote_string function
        ungetc(isQuote, stdin); 
        scan_quote_string(strAux);

        d->crimeDate = completeSetString(strAux, crimeDateLen);
        getc(stdin); // reading the space character
    }
    else { // string read == NULO
        d->crimeDate = NULL;
        char garbage[60];
        scanf("%s", garbage);
        getc(stdin); // reading the space character
    }

    // reading article
    // its never goint to have quote, thusfore we can use a simple string compare
    strAux = readMember(stdin, ' ');
    if (strncmp(strAux, "NULO", 4) == 0)
        d->articleNumber = -1;
    else
        d->articleNumber = atoi(strAux);


    // reading crime place
    // using the logic of scanquote from before
    isQuote = getc(stdin);
    if (isQuote == quote) {
        strAux = (char *)malloc(sizeof(char) * 50);

        ungetc(isQuote, stdin);
        scan_quote_string(strAux);

        d->crimePlace = strAux;
        getc(stdin);
    }
    else { // string read == NULO
        d->crimePlace = NULL;
        char garbage[60];
        scanf("%s", garbage);
        getc(stdin);
    }

    //reading description
    // using the logic of scanquote from before
    isQuote = getc(stdin);
    if (isQuote == quote) {
        strAux = (char *)malloc(sizeof(char) * 50);

        ungetc(isQuote, stdin);
        scan_quote_string(strAux);

        d->crimeDescription = strAux;
        getc(stdin);
    }
    else { // string read == NULO
        d->crimeDescription = NULL;
        char garbage[60];
        scanf("%s", garbage);
        getc(stdin);
    }

    //reading telephone brand
    // using the logic of scanquote from before
    isQuote = getc(stdin);
    if (isQuote == quote) {
        strAux = (char *)malloc(sizeof(char) * 50);

        ungetc(isQuote, stdin);
        scan_quote_string(strAux);

        d->telephoneBrand = completeSetString(strAux, telephoneBrandLen);
        getc(stdin);
    }
    else { // string read == NULO
        d->telephoneBrand = NULL;
        char *garbage = readMember(stdin, '\n');
        free(garbage);
    }

    return d;
}

/*
* Function that given a name of a member of data register
* returns the int value of that field
*/
int selectIntegerMember(char *memberName, Data *reg) {
    if (strncmp("idCrime", memberName, 7) == 0)
        return reg->crimeID;
    else
        return reg->articleNumber;
}

/*
* Function that given a name of a member of data register
* returns the string  of that field 
*/
char *selectStrMember(char *memberName, Data *reg) {
    if(strncmp(memberName, "dataCrime", 9) == 0) {
        return reg->crimeDate;
    }
    else if(strncmp(memberName, "lugarCrime", 10) == 0) { 
        return reg->crimePlace;
    }
    else if(strncmp(memberName, "marcaCelular", 12) == 0) { 
        return reg->telephoneBrand;
    }
    else {//(strncmp(memberName, "descricaoCrime", 14) == 0) 
        return reg->crimeDescription;
    }
}

// return the difference beetween the old and new size
int sizeTest(Data *reg, char *memberName, char *newStr, int newInt) {
    if (strncmp(newStr, "NULO", 4) == 0) {
        newStr = NULL;
    }

    if(strncmp(memberName, "lugarCrime", 10) == 0) { 
        return stringLenght(reg->crimePlace) - stringLenght(newStr);
    }
    else if (strncmp(memberName, "descricaoCrime", 14) == 0) {
        return stringLenght(reg->crimeDescription) - stringLenght(newStr);
    }
    
    return 0;
}

//return the difference beetween the old - new size
void updateReg(Data *reg, char *memberName, char *newStr, int newInt) {

    if (isIntegerMember(memberName)) {
        if (strncmp("idCrime", memberName, 7) == 0) {
            reg->crimeID = newInt;
        }   
        else { // memberName == articleNumber
            reg->articleNumber = newInt;
        }
    }
    else {
        if (strncmp(newStr, "NULO", 4) == 0)
            newStr = NULL;

        if(strncmp(memberName, "dataCrime", 9) == 0) {
            reg->crimeDate = newStr;
        }
        else if(strncmp(memberName, "lugarCrime", 10) == 0) { 
            reg->crimePlace = newStr;
        }
        else if(strncmp(memberName, "marcaCelular", 12) == 0) { 
            reg->telephoneBrand = newStr;
        }
        else {//(strncmp(memberName, "descricaoCrime", 14) == 0) 
            reg->crimeDescription = newStr;
        }
    }
}

int registerSize(Data *reg) {
    return bytesFixedMember + stringLenght(reg->crimeDescription) + stringLenght(reg->crimePlace) + 2;
}

void writeUpdatedRegister(FILE *input, Data *reg, long long int byteoff, int oldSize) {
    fseek(input, byteoff, SEEK_SET);
    int newSize = writeRegister(input, reg) + bytesFixedMember;

    fseek(input, -1, SEEK_CUR); // goes before # written

    int trashPositions = oldSize - newSize;
    //printf("old size: %d\nnew size: %d\n", oldSize, newSize);
    //printf("trash positions: %d\n", trashPositions);
    char trash = '$';
    for(int i=0; i<trashPositions; i++) {
        fwrite(&trash, sizeof(char), 1, input);
    }
}
