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


char *readMember(FILE *input) {

    //static variable used to keep record of the last char read in the file
    //even when the function has finished it will be useful to treat exceptions cases
    static char previousChar = 'a';

    int size = 25;
    int n_char = 0;
    int flagMissingData = 0;
    char *str = (char *)malloc(sizeof(char) * size);
    char *nullStr = nulo;

    char aux = fgetc(input);
    str[n_char++] = aux;

    while (!feof(input)) {
    
        //alocando mais espaco caso necessario
        if(n_char == size) {
            size *= 2;
            str = realloc(str, sizeof(char) * size);
        }

        //fazendo as verificacoes dos campos dos registros
        if (aux == ',' || aux == EOF || aux == '\n') {
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

    tmpData = readMember(input);
    if (tmpData == NULL) return NULL;
    tmpRegister->idCrime = atoi(tmpData);
    
    tmpData = readMember(input);
    tmpRegister->dataCrime = tmpData;

    tmpData = readMember(input);
    if (tmpData == NULL) { //dealing with missing data
        tmpRegister->numeroArtigo = -1;
    } 
    else {
        tmpRegister->numeroArtigo = atoi(tmpData);
    }

    tmpData = readMember(input);
    tmpRegister->lugarCrime = tmpData;

    tmpData = readMember(input);
    tmpRegister->descricaoCrime = tmpData;

    tmpData = readMember(input);
    tmpRegister->marcaCelular = tmpData;

    tmpRegister->removido = '0';

    return tmpRegister;
}

//reading and discarding the first row of the csv file
void readCsvHeader(FILE *input) {
    char *tmpData = NULL;

    for (int i = 0; i < numCampos; i++) {
        tmpData = readMember(input);
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
    writeHeader(output, h);

    int nReg = 0;
    Data *reg = NULL;

    readCsvHeader(input);
    do {
        reg = readRegister(input);
        if (reg == NULL)
            break;

        writeRegister(output, reg);
        add1RegArq(h);

    } while (!feof(input));

    updateHeader(output, h);

    fclose(output);

    return output; //returns file (.bin)
}

void completeSetString(char *str, int lenStr) {
    if (str == NULL) {
        str = (char *)malloc(sizeof(char) * lenStr);
        if (str == NULL) {
            fprintf(stderr, "nao aloquei mem\n");
        }
    }

    for (int i = strlen(str); i < lenStr; i++){
        str[i] = '$';
    }
}

void writeRegister(FILE *output, Data *tmpRegister) {
    char stringDelimiter = '|';
    char registerDelimiter = '#';
    int writeStrDelimiter = 1;

    fwrite(&(tmpRegister->removido), sizeof(char), 1, output);
    fwrite(&(tmpRegister->idCrime), sizeof(int), 1, output);

    //verificando dataCrime - tam fixo
    completeSetString(tmpRegister->dataCrime, lenDataCrime);
    fwrite(tmpRegister->dataCrime, lenDataCrime, 1, output);
    free(tmpRegister->dataCrime);

    fwrite(&(tmpRegister->numeroArtigo), sizeof(int), 1, output);

    //verificando marcaCelular - tam fixo
    /*if (tmpRegister->marcaCelular == NULL) {
        tmpRegister->marcaCelular = (char *)malloc(sizeof(char) * lenMarcaCelular);
    }
    for(int i = strlen(tmpRegister->marcaCelular); i<lenMarcaCelular; i++){
        tmpRegister->marcaCelular[i] = '$';
    }*/
    completeSetString(tmpRegister->marcaCelular, lenMarcaCelular);
    fwrite(tmpRegister->marcaCelular, lenMarcaCelular, 1, output);

    //verificando lugarCrime - tam variavel
    if(tmpRegister->lugarCrime == NULL){
        writeStrDelimiter = 0;
        tmpRegister->lugarCrime = (char *)malloc(sizeof(char));
        tmpRegister->lugarCrime = "|";
    }
    fwrite(tmpRegister->lugarCrime, strlen(tmpRegister->lugarCrime), 1, output);
    if (writeStrDelimiter)
        fwrite(&stringDelimiter, 1, 1, output);
    writeStrDelimiter = 1;

    //verificando descricaoCrime - tam variavel
    if(tmpRegister->descricaoCrime == NULL){
        tmpRegister->descricaoCrime = (char *)malloc(sizeof(char));
        tmpRegister->descricaoCrime = "|";
        writeStrDelimiter = 0;
    }
    fwrite(tmpRegister->descricaoCrime, strlen(tmpRegister->descricaoCrime), 1, output);
    if (writeStrDelimiter)
        fwrite(&stringDelimiter, 1, 1, output);

    fwrite(&registerDelimiter, 1, 1, output);
    
}