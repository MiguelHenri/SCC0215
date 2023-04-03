#ifndef HEADER_H
#define HEADER_H

typedef struct header Header;

Header *createHeader();
void writeHeader(FILE *input, Header *h);
void add1RegArq(Header *h);
void setNroRegArq(Header *h, int num);
void updateHeader(FILE *output, Header *h);

#endif