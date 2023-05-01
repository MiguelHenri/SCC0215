#include <stdio.h>
#include <stdlib.h>
#include "header.h"

struct header {
    char status;
    long long int nextByteOffset;
    int numFileRegisters;
    int numRemovedRegisters;
};

/*
* Creates a header struct, initialize it with default values and returns its pointer
*/
Header *createHeader() {
    Header *h = (Header *)malloc(sizeof(Header));
    if (h == NULL) return NULL;
    
    h->status = '1';
    h->nextByteOffset = 0;
    h->numFileRegisters = 0;
    h->numRemovedRegisters = 0;

    return h;
}

void add1FileReg(Header *h) {
    if (h == NULL) return;
    
    h->numFileRegisters++;
}

/*
* This function swaps the value of the header status from 1 to 0 and vice versa
*/
void updateHeaderStatus(Header *h) {
    if (h == NULL) return;
    
    if (h->status == '0')
        h->status = '1';
    else    
        h->status = '0';
}

/*
* Function utilized to write every member of the header struct into a file
* It returns the amount of bytes written
*/
int writeHeader(FILE *output, Header *h) {
    if (output == NULL || h == NULL) return 0;

    fwrite(&(h->status), sizeof(char), 1, output);
    fwrite(&(h->nextByteOffset), sizeof(long long int), 1, output);
    fwrite(&(h->numFileRegisters),sizeof(int), 1, output);
    fwrite(&(h->numRemovedRegisters), sizeof(int), 1, output);

    return bytesHeader;
}

/*
* Function utilized to overwrite the header struct in a given file
*/
void updateHeader(FILE *output, Header *h) {
    if (output == NULL || h == NULL) return;

    updateHeaderStatus(h);
    fseek(output, 0, SEEK_SET);
    writeHeader(output, h);
}

void addByteOffset(Header *h, int n) {
    h->nextByteOffset += n;
}

/*
* ARRUMAR ESSA FUNCAO !!! 
* Function utilized to read the header struct from a given file and deal 
* with exception cases
* It returns 1 if everything is fine and 0 if there is a problem
*/
int readHeaderBinary(FILE *input) {
    if (input == NULL) return 0;
    
    char charAux;
    long long int llintAux;
    int intAux;

    fread(&charAux, sizeof(char), 1, input);
    if (charAux == '0') {
        FILE_ERROR;
        return 0;
    }

    fread(&llintAux, sizeof(long long int), 1, input);

    fread(&intAux, sizeof(int), 1, input);
    if (intAux == 0) {
        REGISTER_ERROR;
        return 0;
    }

    fread(&intAux, sizeof(int), 1, input);

    return 1;
}

int verifyFileIntegrity(Header *h) {
    return h->status == '1';
}