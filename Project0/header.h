#ifndef HEADER_H
#define HEADER_H

typedef struct header Header;

#define bytesHeader 17

Header *createHeader();
int writeHeader(FILE *input, Header *h);
void add1RegArq(Header *h);
void setNroRegArq(Header *h, int num);
void updateHeader(FILE *output, Header *h);
void updateHeaderStatus(Header *h);
void addByteOffset(Header *h, int n);


#endif