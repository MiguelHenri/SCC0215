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

/*
* This function receives a .bin file input with registers
* It indexes the registers according to the index member name (ex: idCrime)
* and creates and writes an index file named 'nameIndexFile'
*/
void createIndexFile(FILE *input, char *memberName, char *indexType, char *nameIndexFile) {
    FILE *index = fopen(nameIndexFile, "wb");
    if (index == NULL) {
        FILE_ERROR;
        return;
    }

    IndexHeader *h = createIndexHeader();
    IndexData *arr = createIndexArr(input, h, indexType, memberName);

    // writes index file after sorting index arr data
    writeFileIndex(index, arr, h, memberName);
    fclose(index);
}

/*
* This function receives a .bin file input with registers
* and a .bin index file named 'nameIndexFile', indexed by 'memberName'
* It searches numberSearches registers and prints its data
* It priorizes searching by index (if possible)
*/
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
    fclose(indexFile);

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
        //returning file pointer to the beggining
        fseek(input, 0, SEEK_SET);
    }

}

/*
* This function receives a .bin file input with registers
* and a .bin index file named 'nameIndexFile', indexed by 'memberName'
* It searches numberDeletions registers, and delete they in both files if found
*/
void deleteRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberDeletions) {
    // reading data header
    Header *h = readHeaderBinary(input);
    if (!h) return;

    FILE *indexFile = fopen(nameIndexFile, "rb");

    // reading header data, verifies file consistency
    IndexHeader *indexHeader = createIndexHeader();

    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    fclose(indexFile);

    for(int i=0; i<numberDeletions; i++) {
        // reading data to be deleted
        int pairs;
        Search *s = createSearchArr(input, &pairs);

        // finding registers to be deleted and its offset
        Result *res = superSearch(input, memberName, indexDataArr, indexHeader, s, pairs, h);

        // deleting registers found
        indexDataArr = superDelete(input, res, indexDataArr, indexHeader, h);
    }

    // updating index file
    indexFile = fopen(nameIndexFile, "wb");
    writeFileIndex(indexFile, indexDataArr, indexHeader, memberName);
    fclose(indexFile);

    // updating header in data file
    fseek(input, 0, SEEK_SET);
    writeHeader(input, h);
    
}

/*
* This function receives a .bin file input with registers
* and a .bin index file named 'nameIndexFile', indexed by 'memberName'
* It inserts numberInsert registers, to both files
*/
void insertRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numberInsert) {
    
    // opening index file .bin
    FILE *indexFile = fopen(nameIndexFile, "rb+");

    // reading header data, also verifies file consistency
    IndexHeader *indexHeader = createIndexHeader();

    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    fclose(indexFile);

    Header *h = readHeaderBinary(input);
    updateHeader(input, h);
    for (int i = 0; i < numberInsert; i++) {
        Data *reg = readRegisterStdin2();
        
        long long int currentByteOff = insertRegisterInBinFile(input, reg, h);
        if (isIntegerMember(memberName)) {

            indexDataArr = appendIndexArray(indexDataArr, indexHeader, memberName, selectIntegerMember(memberName, reg), NULL, currentByteOff);
        }
        else {
            indexDataArr = appendIndexArray(indexDataArr, indexHeader, memberName, -1, selectStrMember(memberName, reg), currentByteOff);
        }
    }

    // updating data file header
    updateHeader(input, h);

    // updating index file 
    indexFile = fopen(nameIndexFile, "wb");
    writeFileIndex(indexFile, indexDataArr, indexHeader, memberName);
    fclose(indexFile);
}

/*
* This function receives a .bin file input with registers
* and a .bin index file named 'nameIndexFile', indexed by 'memberName'
* It searches numUpdates registers, and update them if found
*/
void updateRegister(FILE *input, char *memberName, char *indexType, char *nameIndexFile, int numUpdates) {
    // reading data header
    Header *h = readHeaderBinary(input);
    if (!h) return;

    FILE *indexFile = fopen(nameIndexFile, "rb+");

    // reading header data, also verifies file consistency
    IndexHeader *indexHeader = createIndexHeader();
    readIndexHeader(indexFile, indexHeader);

    // reading index file data
    IndexData *indexDataArr = readFileIndex(indexFile, memberName, indexHeader);
    fclose(indexFile);

    for (int i = 0; i < numUpdates; i++) {
        int pairsSearch;
        // reading input with data to be searched
        Search *s = createSearchArr(input, &pairsSearch);    

        // finding registers to be updated and its offset
        Result *res = superSearch(input, memberName, indexDataArr, indexHeader, s, pairsSearch, h);
        
        // reading update values
        int pairsUpdate;
        Search *update = createSearchArr(stdin, &pairsUpdate);
        
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