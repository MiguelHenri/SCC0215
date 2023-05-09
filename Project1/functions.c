#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

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
* This function receives a .bin file 'from'
* it reads the file and prints file data collected in the terminal
*/
void selectFrom(FILE *from) {
    if (from == NULL) return;

    Data *reg = NULL;
    int flagError;

    Header *h = readHeaderBinary(from);
    if (flagError == 0) // inconsistent file
        return;
    
    while (!feof(from)) { // reads registers one by one
        reg = readBinaryRegister(from);
        printData(reg);
    }
}

// funcionalidade 3
void createIndexFile(FILE *input, char *memberName, char *indexType, char *nameIndexFile) {
    FILE *index = fopen(nameIndexFile, "wb");
    if (index == NULL) {
        FILE_ERROR;
        return;
    }

    IndexHeader *h = createIndexHeader();
    IndexData *arr = createIndexArr(input, h, indexType, memberName);

    writeFileIndex(index, arr, h, memberName);
    fclose(index);

    binarioNaTela(nameIndexFile);
    
}

// funcionaliade 4
void searchInBinaryFile(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberSearches) {

    FILE *indexFile = fopen(nameIndexFile, "rb");

    // reading header data
    // verificar no futuro o status e atribuir erro ou nao
    IndexHeader *indexHeader = createIndexHeader();
    int intAux; char charAux;

    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    fclose(indexFile);
    //printIndex(indexDataArr, indexHeader);

    for (int i = 0; i < numberSearches; i++) {
        //printf("iteracao %d\n", i);
        int pairs;
        int lenArrByteOffset;
        Search *s = createSearchArr(input, &pairs);
        //printf("crieir arr de wanted\n");

        //check if will be search by index or linear search
        long long int *arrByteOffset = NULL;
        long long int *test = NULL;
        if (isMemberInIndex(s, 0, memberName)) {
            //printf("entrei aui\n");
            arrByteOffset = searchInIndexArr(indexDataArr, indexHeader, s, 0, memberName, &lenArrByteOffset);

            //funcionando atq aqui
            arrByteOffset = verifyingRegRequirements(input, arrByteOffset, s, pairs, &lenArrByteOffset);
        }
        else {
            arrByteOffset = search2(input, s, pairs, &lenArrByteOffset);
        }

        
        fprintf(stderr, "Resposta para a busca %d\n", i+1);
        
        if (arrByteOffset == NULL || lenArrByteOffset == 0) {
            printf("Registro inexistente.\n");
        }
        else {
            for (int j = 0; j < lenArrByteOffset; j++) {
                fseek(input, arrByteOffset[j], SEEK_SET);
                Data *d = readBinaryRegister(input);
                printData(d);
            }
        }

        //returning file pointer to the beggining
        fseek(input, 0, SEEK_SET);
    }

}

void insertRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberInsert) {
    
    
    FILE *indexFile = fopen(nameIndexFile, "rb");

    // reading header data
    // verificar no futuro o status e atribuir erro ou nao
    IndexHeader *indexHeader = createIndexHeader();
    int intAux; char charAux;

    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    fclose(indexFile);

    Header *h = readHeaderBinary(input);

    for (int i = 0; i < numberInsert; i++) {
        Data *reg = readRegisterStdin();
        //printData(reg);
        insertRegisterInBinFile(input, reg, h);
    }

}