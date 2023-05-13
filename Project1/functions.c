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
    Header *h = readHeaderBinary(input);
    if (!h) return;

    FILE *indexFile = fopen(nameIndexFile, "rb");

    // reading header data
    // verificar no futuro o status e atribuir erro ou nao
    IndexHeader *indexHeader = createIndexHeader();
    int intAux; char charAux;

    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    // printIndex(indexDataArr, indexHeader);
    fclose(indexFile);
    //printf("consegui criar arr de indice\n");

    for (int i = 0; i < numberSearches; i++) {
        int pairs;
        Search *s = createSearchArr(input, &pairs);
        Result *res = superSearch(input, memberName, indexDataArr, indexHeader, s, pairs, h);
        
        printf("Resposta para a busca %d\n", i+1);
        
        if (res == NULL || getResLenght(res) == 0) {
            printf("Registro inexistente.\n");
        }
        else { //arrByteOffset != NULL
            for (int j = 0; j < getResLenght(res); j++) {
                fseek(input, getResByteOffset(res, j), SEEK_SET);
                Data *d = readBinaryRegister(input);
                printData(d);
            }
        }
        // printf("\n\n");
        //returning file pointer to the beggining
        fseek(input, 0, SEEK_SET);
    }

}

// funcionalidade 5
void deleteRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberDeletions) {
    // reading data header
    Header *h = readHeaderBinary(input);
    if (!h) return;

    FILE *indexFile = fopen(nameIndexFile, "rb");

    // reading header data
    // verificar no futuro o status e atribuir erro ou nao
    IndexHeader *indexHeader = createIndexHeader();

    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    // printIndex(indexDataArr, indexHeader);
    fclose(indexFile);

    for(int i=0; i<numberDeletions; i++) {
        // printf("delecao de numero: %d\n", i+1);
        int pairs;
        Search *s = createSearchArr(input, &pairs);
        // finding registers to be deleted and its offset

        //fprintf(stderr, "crirei vetor de seacr\n");
        Result *res = superSearch(input, memberName, indexDataArr, indexHeader, s, pairs, h);

        // deleting registers found
        indexDataArr = superDelete(input, res, indexDataArr, indexHeader, h);
    }

    // updating index file
    indexFile = fopen(nameIndexFile, "wb");
    writeFileIndex(indexFile, indexDataArr, indexHeader, memberName);
    fclose(indexFile);

    // printf("tem %d no indice\n", getIndexArrLen(indexHeader));
    // updating header in data file
    fseek(input, 0, SEEK_SET);
    writeHeader(input, h);
    
}

// funcionalidade 6
void insertRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberInsert) {
    
    // opening index file .bin
    FILE *indexFile = fopen(nameIndexFile, "rb+");

    // reading header data
    // verificar no futuro o status e atribuir erro ou nao
    IndexHeader *indexHeader = createIndexHeader();

    readIndexHeader(indexFile, indexHeader);
    // printIndexHeader(indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    fclose(indexFile);

    Header *h = readHeaderBinary(input);
    updateHeader(input, h);
    for (int i = 0; i < numberInsert; i++) {
        Data *reg = readRegisterStdin2();
        //printData(reg);
        
        long long int currentByteOff = insertRegisterInBinFile(input, reg, h);
        if (isIntegerMember(memberName)) {

            indexDataArr = appendIndexArray(indexDataArr, indexHeader, memberName, selectIntegerMember(memberName, reg), NULL, currentByteOff);
        }
        else {
            indexDataArr = appendIndexArray(indexDataArr, indexHeader, memberName, -1, selectStrMember(memberName, reg), currentByteOff);
        }
    }

    updateHeader(input, h);
    indexFile = fopen(nameIndexFile, "wb");
    writeFileIndex(indexFile, indexDataArr, indexHeader, memberName);
    // printIndexHeader(indexHeader);
    // printIndex(indexDataArr, indexHeader);
    fclose(indexFile);
}

//funcionalidade 7
void updateRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numUpdates) {
    // reading data header
    Header *h = readHeaderBinary(input);
    if (!h) return;

    FILE *indexFile = fopen(nameIndexFile, "rb+");

    // reading header data
    // verificar no futuro o status e atribuir erro ou nao
    IndexHeader *indexHeader = createIndexHeader();
    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    fclose(indexFile);
    //printf("criei arr indice\n");


    for (int i = 0; i < numUpdates; i++) {
        int pairsSearch;
        // reading input with data to be searched
        Search *s = createSearchArr(input, &pairsSearch);
        //printSearchArray(s);
        //printf("pairs search %d\n", pairsSearch);
        

        // finding registers to be updated and its offset
        Result *res = superSearch(input, memberName, indexDataArr, indexHeader, s, pairsSearch, h);
        
        // reading update values
        int pairsUpdate;
        Search *update = createSearchArr(stdin, &pairsUpdate);
        //printf("pairs update %d\n", pairsUpdate);
        //printSearchArray(update);
        
        indexDataArr = superUpdate(input, update, res, indexDataArr, indexHeader, h, memberName);
    }

    // updating index file
    indexFile = fopen(nameIndexFile, "wb");
    writeFileIndex(indexFile, indexDataArr, indexHeader, memberName);
    fclose(indexFile);
    // updating header in data file
    fseek(input, 0, SEEK_SET);
    writeHeader(input, h);
}