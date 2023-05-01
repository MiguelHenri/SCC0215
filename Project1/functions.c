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

    flagError = readHeaderBinary(from);
    if (flagError == 0) // inconsistent file
        return;
    
    while (!feof(from)) { // reads registers one by one
        reg = readBinaryRegister(from);
        printData(reg);
    }
}

//funcionaliade 4
void searchInBinaryFile(FILE *input) {
    getc(stdin);
    char *indexMember = readMember(stdin, ' ');
    char *dataTypeIndexMember = readMember(stdin, ' ');
    char *binaryFileIndex = readMember(stdin, ' ');
    int numberSearches;
    scanf(" %d", &numberSearches);

    for (int i = 0; i < numberSearches; i++) {
        int pairs;
        int lenArrByteOffset;
        Search *s = createSearchArr(input, &pairs);

        long long int *arrByteOffset = search2(input, s, pairs, &lenArrByteOffset);
        
        printf("Resposta para a busca %d\n", i+1);
        
        if (arrByteOffset == NULL || lenArrByteOffset == 0) {
            printf("Registro inexistente.\n");
        }
        else {
            for (int j = 0; j < lenArrByteOffset; j++) {
                //ta faltando 7 wtfffffff
                fseek(input, arrByteOffset[j], SEEK_SET);
                Data *d = readBinaryRegister(input);
                printData(d);
            }
        }

        //returning file pointer to the beggining
        fseek(input, 0, SEEK_SET);
    }

}