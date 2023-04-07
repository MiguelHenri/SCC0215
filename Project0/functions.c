#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "header.h"

struct data {
    char removido;
    int idCrime;
    char *dataCrime;
    int numeroArtigo;
    char *marcaCelular;
    char *lugarCrime;
    char *descricaoCrime;
};


char *readMember(FILE *input, char delimiter) {

    //static variable used to keep record of the last char read in the file
    //even when the function has finished it will be useful to treat exceptions cases
    static char previousChar = 'a';

    int size = 25;
    int n_char = 0;
    int flagMissingData = 0;
    char *str = (char *)malloc(sizeof(char) * size);

    char aux = fgetc(input);
    str[n_char++] = aux;

    while (!feof(input)) {
    
        //alocando mais espaco caso necessario
        if(n_char == size) {
            size *= 2;
            str = realloc(str, sizeof(char) * size);
        }

        //fazendo as verificacoes dos campos dos registros
        if (aux == delimiter || aux == EOF || aux == '\n') {
            if (previousChar == ',') {
                flagMissingData = 1;
            }
            previousChar = aux;
            str[--n_char] = '\0';
            break;
        }

        //lendo caracter e armazenado anterior
        previousChar = aux;
        aux = fgetc(input);

        str[n_char++] = aux;
    }


    if (flagMissingData || str[0] == '\0') 
        return NULL;
    else    
        return str;
}

Data *readRegister(FILE *input) {
    Data *tmpRegister = (Data *)malloc(sizeof(Data));
    if (tmpRegister == NULL) {
        printf("Erro de alocacao de memoria\n");
        return NULL;
    }

    char *tmpData = NULL;
    char delimiter = ',';

    tmpData = readMember(input, delimiter);
    if (tmpData == NULL) return NULL;
    tmpRegister->idCrime = atoi(tmpData);
    
    tmpData = readMember(input, delimiter);
    tmpRegister->dataCrime = tmpData;

    tmpData = readMember(input, delimiter);
    if (tmpData == NULL) { //dealing with missing data
        tmpRegister->numeroArtigo = -1;
    } 
    else {
        tmpRegister->numeroArtigo = atoi(tmpData);
    }

    tmpData = readMember(input, delimiter);
    tmpRegister->lugarCrime = tmpData;

    tmpData = readMember(input, delimiter);
    tmpRegister->descricaoCrime = tmpData;

    tmpData = readMember(input, delimiter);
    tmpRegister->marcaCelular = tmpData;

    tmpRegister->removido = '0';

    return tmpRegister;
}

//reading and discarding the first row of the csv file
void readCsvHeader(FILE *input) {
    char *tmpData = NULL;
    char delimiter = ',';

    for (int i = 0; i < numCampos; i++) {
        tmpData = readMember(input, delimiter);
        free(tmpData);
    }
}

FILE *createTable(FILE *input) {
    char nameOutput[100];
    scanf("%s", nameOutput);
    
    FILE *output = fopen(nameOutput, "wb");
    if (output == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return NULL;
    }

    Header *h = createHeader();
    updateHeaderStatus(h); //switching to inconsistent status
    //writes header data and sums its size
    addByteOffset(h, writeHeader(output, h));

    int nReg = 0;
    Data *reg = NULL;

    readCsvHeader(input);
    do {
        reg = readRegister(input);
        if (reg == NULL)
            break;

        addByteOffset(h, writeRegister(output, reg) + bytesFixedMember);

        add1RegArq(h);

    } while (!feof(input));

    updateHeader(output, h);

    fclose(output);

    return output; //returns file (.bin)
}

char *completeSetString(char *str, int lenStr) {
    if (str == NULL) {
        str = (char *)malloc(sizeof(char) * lenStr);
    }
    else {
        str = realloc(str, lenStr);
    }

    for (int i = strlen(str); i < lenStr; i++) {
        str[i] = '$';
    }

    return str;
}

char *completeUnsetString(char *str, int *flag) {
    if (str != NULL) return str;

    str = (char *)malloc(sizeof(char));
    str = "|";
    *flag = 0;

    return str;
}

int writeRegister(FILE *output, Data *tmpRegister) {
    int variableSize = 0;
    char stringDelimiter = '|';
    char registerDelimiter = '#';

    fwrite(&(tmpRegister->removido), sizeof(char), 1, output);
    fwrite(&(tmpRegister->idCrime), sizeof(int), 1, output);

    //verificando dataCrime - tam fixo

    tmpRegister->dataCrime = completeSetString(tmpRegister->dataCrime, lenDataCrime);

    fwrite(tmpRegister->dataCrime, lenDataCrime, 1, output);
    //free(tmpRegister->dataCrime);

    fwrite(&(tmpRegister->numeroArtigo), sizeof(int), 1, output);

    //verificando marcaCelular - tam fixo

    tmpRegister->marcaCelular = completeSetString(tmpRegister->marcaCelular, lenMarcaCelular);

    fwrite(tmpRegister->marcaCelular, lenMarcaCelular, 1, output);

    int writeStrDelimiter = 1;
    //verificando lugarCrime - tam variavel
    tmpRegister->lugarCrime = completeUnsetString(tmpRegister->lugarCrime, &writeStrDelimiter);
    fwrite(tmpRegister->lugarCrime, strlen(tmpRegister->lugarCrime), 1, output);
    variableSize += strlen(tmpRegister->lugarCrime);
    if (writeStrDelimiter) {
        fwrite(&stringDelimiter, 1, 1, output);
        variableSize++;
    }

    writeStrDelimiter = 1;
    //verificando descricaoCrime - tam variavel
    tmpRegister->descricaoCrime = completeUnsetString(tmpRegister->descricaoCrime, &writeStrDelimiter);
    fwrite(tmpRegister->descricaoCrime, strlen(tmpRegister->descricaoCrime), 1, output);
    variableSize += strlen(tmpRegister->descricaoCrime);
    if (writeStrDelimiter) {
        fwrite(&stringDelimiter, 1, 1, output);
        variableSize++;
    }

    fwrite(&registerDelimiter, 1, 1, output);

    return variableSize; 
}

Data *readBinaryFile(FILE *input) {
    Data *tmp = (Data *)malloc(sizeof(Data));
    if (tmp == NULL) return NULL;

    char *strAux1 = (char *)malloc(sizeof(char) * lenDataCrime);
    char *strAux2 = (char *)malloc(sizeof(char) * lenDataCrime);
    char charAux;
    int intAux;
    char delimiter = '|';

    fread(&charAux, sizeof(char), 1, input);
    tmp->removido = charAux;

    fread(&intAux, sizeof(int), 1, input);
    tmp->idCrime = intAux;

    fread(strAux2, sizeof(char) * lenDataCrime, 1, input);
    tmp->dataCrime = strAux2;

    fread(&intAux, sizeof(int), 1, input);
    tmp->numeroArtigo = intAux;

    fread(strAux1, sizeof(char) * lenMarcaCelular, 1, input);
    tmp->marcaCelular = strAux1;

    strAux1 = readMember(input, delimiter);
    tmp->lugarCrime = strAux1;

    strAux1 = readMember(input, delimiter);
    tmp->descricaoCrime = strAux1;

    fread(&charAux, sizeof(char), 1, input);//reading register delimiter

    return tmp;
}

void printData(Data *d) {
    if (d->dataCrime[0] == '\0') return;

    printf("%d, ", d->idCrime);
    //tratando o $
    if(d->dataCrime[0] == '$') {
        printf("NULO, ");
    }
    else {
        int i = 0;
        while (i < lenDataCrime && d->dataCrime[i] != '$')
            printf("%c", d->dataCrime[i++]);
        printf(", ");
    }
    
    if (d->numeroArtigo == -1) 
        printf("NULO, ");
    else
        printf("%d, ", d->numeroArtigo);

    if (d->lugarCrime == NULL)
        printf("NULO, ");
    else
        printf("%s, ", d->lugarCrime);
        
    if (d->descricaoCrime == NULL)
        printf("NULO, ");
    else
        printf("%s, ", d->descricaoCrime);
    //tratando o $
    if(d->marcaCelular[0] == '$')
        printf("NULO");
    else {
        int i = 0;
        while (i < lenMarcaCelular && d->marcaCelular[i] != '$')
            printf("%c", d->marcaCelular[i++]);
    }
    printf("\n");
}

void selectFrom(FILE *from) {
    Data *reg = NULL;

    fseek(from, bytesHeader, SEEK_SET);
    
    while (!feof(from)) {
        reg = readBinaryFile(from);
        printData(reg);
    }
}