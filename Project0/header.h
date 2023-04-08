#ifndef HEADER_H
#define HEADER_H

typedef struct header Header;

#define bytesHeader 17
#define REGISTER_ERROR printf("Non-existent register.\n")
#define FILE_ERROR printf("File processing failed.\n")

Header *createHeader();
void add1FileReg(Header *h);
void updateHeaderStatus(Header *h);
int writeHeader(FILE *input, Header *h);
void updateHeader(FILE *output, Header *h);
void addByteOffset(Header *h, int n);
int readHeaderBinary(FILE *input);


#endif