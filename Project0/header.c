#include <stdio.h>
#include <stdlib.h>
#include "header.h"

struct header {
    char status;
    long long int proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};

Header *createHeader() {
    Header *h = (Header *)malloc(sizeof(Header));
    if (h == NULL) return NULL;
    
    h->status = '1';
    h->proxByteOffset = 0;
    h->nroRegArq = 0;
    h->nroRegRem = 0;

    return h;
}

void add1RegArq(Header *h) {
    if (h == NULL) return;
    
    h->nroRegArq++;
}

void setNroRegArq(Header *h, int num) {
    if (h == NULL) return;

    h->nroRegArq = num;
}

void updateHeaderStatus(Header *h) {
    if (h == NULL) return;
    
    if (h->status == '0')
        h->status = '1';
    else    
        h->status = '0';
}

int writeHeader(FILE *output, Header *h) {
    if (output == NULL || h == NULL) return 0;

    fwrite(&(h->status), sizeof(char), 1, output);
    fwrite(&(h->proxByteOffset), sizeof(long long int), 1, output);
    fwrite(&(h->nroRegArq),sizeof(int), 1, output);
    fwrite(&(h->nroRegRem), sizeof(int), 1, output);

    return 17;
}

void updateHeader(FILE *output, Header *h) {
    if (output == NULL || h == NULL) return;

    updateHeaderStatus(h);
    fseek(output, 0, SEEK_SET);
    writeHeader(output, h);
}

void printNroReg(Header *h) {
    printf("[%c]\n", h->status);
}

void addByteOffset(Header *h, int n) {
    h->proxByteOffset += n;
}