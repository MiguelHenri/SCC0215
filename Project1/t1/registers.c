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

int intMemberCompare(char *searchMember, int searchKey, Data *reg) {

    if (strncmp(searchMember, "idCrime", 7) == 0 &&
    searchKey == reg->crimeID) {
        return 1;
    }
    else if (strncmp(searchMember, "numeroArtigo", 12) == 0 &&
        searchKey == reg->articleNumber) {
        return 1;
    }

    return 0;
}

int strMemberCompare(char *searchMember, char *searchKey, Data *reg) {
    int lenKey = strlen(searchKey);

    if(strncmp(searchMember, "dataCrime", 9) == 0) {
        //stringCompareWithLen(searchKey, reg->crimeDate, lenKey) == 0) {
        if (reg->crimeDate == NULL) return 0;

        //printf("comparando reg: %s[%d] wanted: %s[%d]\n", 
        //reg->crimeDate, (int)strlen(reg->crimeDate), searchKey, lenKey);
        
        return strncmp(reg->crimeDate, searchKey, lenKey) == 0;
    }
    else if(strncmp(searchMember, "lugarCrime", 10) == 0) { 
        //stringCompareWithLen(searchKey, reg->crimePlace, lenKey) == 0) {
        if (reg->crimePlace == NULL) return 0;
        
        //printf("comparando reg: %s[%d] wanted: %s[%d]\n", 
        //reg->crimePlace, (int)strlen(reg->crimePlace), searchKey, lenKey);
        
        return strncmp(reg->crimePlace, searchKey, lenKey) == 0;
    }
    else if(strncmp(searchMember, "marcaCelular", 12) == 0) { 
        //stringCompareWithLen(searchKey, reg->telephoneBrand, lenKey) == 0) {
        if (reg->telephoneBrand == NULL) return 0;

        //printf("comparando reg: %s[%d] wanted: %s[%d]\n", 
        //reg->telephoneBrand, (int)strlen(reg->telephoneBrand), searchKey, lenKey);

        return strncmp(reg->telephoneBrand, searchKey, lenKey) == 0;
    }
    else if(strncmp(searchMember, "descricaoCrime", 14) == 0) { 
        //stringCompareWithLen(searchKey, reg->crimeDescription, lenKey) == 0) {
        if (reg->crimeDescription == NULL) return 0;
        
        //printf("comparando reg: %s[%d] wanted: %s[%d]\n", 
        //reg->crimeDescription, (int)strlen(reg->crimeDescription), searchKey, lenKey);
        
        return strncmp(reg->crimeDescription, searchKey, lenKey) == 0;
    }

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


void insertRegisterInBinFile(FILE *binFile, Data *reg, Header *h) {
    
    //calculating the byteoffset and setting the file to the end
    long long int currentOffset = getNexByteOffset(h);
    fseek(binFile, currentOffset, SEEK_SET);

    //adding the byteoffset to the header
    currentOffset = (writeRegister(binFile, reg) + bytesFixedMember);
    addByteOffset(h, currentOffset);
    add1FileReg(h);
}

Data *readRegisterStdin2() {
    Data *d = (Data *)malloc(sizeof(Data));
    char isQuote;
    char quote = '"';
    char lixo;
    int intAux;

    d->removed = '0';

    fscanf(stdin, "%d ", &intAux);
    d->crimeID = intAux;

    char *strAux = (char *)malloc(sizeof(char) * 50);
    isQuote = getc(stdin);
    printf( "[%c]", isQuote);

    //reading
    if (isQuote == quote) {
        ungetc(isQuote, stdin);
        scan_quote_string(strAux);
        d->crimeDate = completeSetString(strAux, crimeDateLen);
        printf("%s\n", strAux);
        getc(stdin);
    }
    else {
        d->crimeDate = NULL;
        readMember(stdin, ' ');
    }

    isQuote = getc(stdin);
    printf( "[%c]", isQuote);
    if (isQuote == quote) {
        ungetc(isQuote, stdin);
        scan_quote_string(strAux);
        d->crimeDate = completeSetString(strAux, crimeDateLen);
        printf("%s\n", strAux);
        getc(stdin);
    }
    else {
        d->crimeDate = NULL;
        readMember(stdin, ' ');
    }

    printf("acabei\n");
}

Data *readRegisterStdin() {
    Data *d = (Data *)malloc(sizeof(Data));

    int intAux;
    char isQuote;
    char quote = '"';
    char lixo;

    d->removed = '0';

    fscanf(stdin, "%d ", &intAux);
    d->crimeID = intAux;
    fprintf(stderr, "id %d || ", d->crimeID);
    // fprintf(stderr, "crime id: %d ", d->crimeID);


    char *strAux = (char *)malloc(sizeof(char) * crimeDateLen);
    isQuote = getc(stdin);
    fprintf(stderr, "[%c]", isQuote);
    if (isQuote == quote) {
        ungetc(isQuote, stdin);
        scan_quote_string(strAux);
        d->crimeDate = completeSetString(strAux, crimeDateLen);
        printf("%s\n", strAux);
    }
    else {
        d->crimeDate = NULL;
        readMember(stdin, ' ');
    }


    if (d->crimeDate != NULL)printf("data nula\n");
    // fprintf(stderr, "crime date: %s ", d->crimeDate);
    
    lixo = getc(stdin);
    fprintf(stderr,"lixo [%c]", lixo);

    strAux = readMember(stdin, ' ');
    if (strncmp(strAux, "NULO", 4) == 0)
        d->articleNumber = -1;
    else
        d->articleNumber = atoi(strAux);
    fprintf(stderr, "num artigp %d || ", d->articleNumber);
    // fprintf(stderr, "artiho: %d ", d->articleNumber);


    strAux = (char *)malloc(sizeof(char) * 50);
    isQuote = getc(stdin);
    fprintf(stderr, "[%c]", isQuote);
    if (isQuote == quote) {
        ungetc(isQuote, stdin);
        scan_quote_string(strAux);
        d->crimePlace = strAux;
    }
    else {
        d->crimeDate = NULL;
        char *idk = readMember(stdin, ' ');
        fprintf(stderr, "%s\n", idk);
    }
    // if (d->crimePlace != NULL)printf("lugar %s || ", d->crimePlace);
    
    // fprintf(stderr, "crime place: %s ", d->crimePlace);
    lixo = getc(stdin);
    fprintf(stderr, "lixo [%c]", lixo);

    strAux = (char *)malloc(sizeof(char) * 50);
    isQuote = getc(stdin);
    if (isQuote == quote) {
        ungetc(isQuote, stdin);
        scan_quote_string(strAux);
        d->crimeDescription = strAux;
    }
    else {
        d->crimeDate = NULL;
        char *idk = readMember(stdin, ' ');
    }
    // if (d->crimeDescription != NULL)printf("descricao %s ||", d->crimeDescription);
    // fprintf(stderr, "crime desc: %s ", d->crimeDescription);

    lixo = getc(stdin);
    printf("lixo [%c]", lixo);

    strAux = (char *)malloc(sizeof(char) * telephoneBrandLen);
    isQuote = getc(stdin);
    if (isQuote == quote) {
        ungetc(isQuote, stdin);
        scan_quote_string(strAux);
        d->telephoneBrand = completeSetString(strAux, telephoneBrandLen);
    }
    else {
        d->crimeDate = NULL;
        readMember(stdin, ' ');
    }
    // if (d->telephoneBrand != NULL)printf("celular %s", d->telephoneBrand);
    // fprintf(stderr, "cellphone: %s\n", d->telephoneBrand);
    getc(stdin);
    printf("\n");
    return d;
}