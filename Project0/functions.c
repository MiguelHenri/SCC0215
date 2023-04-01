#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

struct data {
    char removido;
    int idCrime;
    char dataCrime[10];
    int numeroArtigo;
    char marcaCelular[12];
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

        //fazendo as verificacoes de ','
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

    if (flagMissingData) {
        free(str);
        return nullStr;
    }
    else {
        str = realloc(str, n_char);
        printf("[%d][%s]\n", n_char, str);
        return str;
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
    
    Data *tmpRegister = (Data *)malloc(sizeof(Data));
    if (tmpRegister == NULL) {
        printf("Erro de alocacao de memoria\n");
        return NULL;
    }

    int nReg = 0;
    char *str_aux = NULL;
    char *strInt = NULL;
    char *strTokAux = NULL;
    char *fileLine = NULL;
    int aaux;


    //lendo a primeira linha, nao sera utilizado esses dados
    printf("li a primeira linha\n");
    for (int i = 0; i < numCampos; i++) {
        str_aux = readMember(input);
        free(str_aux);
    }

    do {
        printf("-----------------------------------------------\n");
        str_aux = readMember(input);
        tmpRegister->idCrime = atoi(str_aux);
        
        str_aux = readMember(input);
        strcpy(tmpRegister->dataCrime, str_aux);

        str_aux = readMember(input);
        if (str_aux[0] == '$') { //dealing with missing data
            tmpRegister->numeroArtigo = -1;
        } 
        else {
            tmpRegister->numeroArtigo = atoi(str_aux);
        }

        str_aux = readMember(input);
        tmpRegister->lugarCrime = str_aux;

        str_aux = readMember(input);
        tmpRegister->descricaoCrime = str_aux;

        str_aux = readMember(input);
        strcpy(tmpRegister->marcaCelular, str_aux);

        tmpRegister->removido = '0';

        writeRegister(output, tmpRegister);

    } while (!feof(input));

    fclose(output);

    return output; //returns file (.bin)
}

void writeRegister(FILE *output, Data *tmpRegister) {
    char stringDelimiter = '|';
    char registerDelimiter = '#';

    fwrite(&(tmpRegister->removido), sizeof(char), 1, output);
    fwrite(&(tmpRegister->idCrime), sizeof(int), 1, output);

    //tratar string nula do tipo "$$$"
    //se tamanho variável recebe pipe |
    //se tamanho fixo preenche com $

    //verificando dataCrime - tam fixo
    for(int i = strlen(tmpRegister->dataCrime); i<lenDataCrime; i++){
        tmpRegister->dataCrime[i] = '$';
    }
    fwrite(tmpRegister->dataCrime, lenDataCrime, 1, output);

    fwrite(&(tmpRegister->numeroArtigo), sizeof(int), 1, output);

    //verificando marcaCelular - tam fixo
    for(int i = strlen(tmpRegister->marcaCelular); i<lenMarcaCelular; i++){
        tmpRegister->marcaCelular[i] = '$';
    }
    fwrite(tmpRegister->marcaCelular, lenMarcaCelular, 1, output);

    //verificando lugarCrime - tam variavel
    if(strcmp(tmpRegister->lugarCrime, nulo) == 0){
        tmpRegister->lugarCrime = (char *)malloc(sizeof(char));
        tmpRegister->lugarCrime = "|";
    }
    fwrite(tmpRegister->lugarCrime, strlen(tmpRegister->lugarCrime), 1, output);
    fwrite(&stringDelimiter, 1, 1, output);

    //verificando descricaoCrime - tam variavel
    if(strcmp(tmpRegister->descricaoCrime, nulo) == 0){
        tmpRegister->descricaoCrime = (char *)malloc(sizeof(char));
        tmpRegister->descricaoCrime = "|";
    }
    fwrite(tmpRegister->descricaoCrime, strlen(tmpRegister->descricaoCrime), 1, output);
    fwrite(&stringDelimiter, 1, 1, output);

    fwrite(&registerDelimiter, 1, 1, output);
    
}

