#ifndef HEADER_H
#define HEADER_H

typedef struct header Header;

#define bytesHeader 17
#define REGISTER_ERROR printf("Registro inexistente.\n")
#define FILE_ERROR printf("Falha no processamento do arquivo.\n")

Header *createHeader();
void sub1FileReg(Header *h);
void add1FileReg(Header *h);
void add1FileRemReg(Header *h);
void updateHeaderStatus(Header *h);
int writeHeader(FILE *input, Header *h);
void updateHeader(FILE *output, Header *h);
void addByteOffset(Header *h, int n);
Header *readHeaderBinary(FILE *input);
int verifyFileIntegrity(Header *h);
int getNumFileRegisters(Header *h);
long long int getNextByteOffset(Header *h);

#endif