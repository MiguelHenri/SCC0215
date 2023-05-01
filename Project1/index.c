#include "index.h"
#include "registers.h"

struct indexHeader {
    char status;
}

struct indexData {
    long long int byteOffset;
    union {
        char *searchKeyStr;
        int searchKeyInt;
    };
};

// input == binary data file
FILE *createIndex(FILE *input, char *nameIndexFile, char *indexType, char *memberName) {
    FILE *indexFile = fopen(nameIndexFile, "wb"); // new index bin file

    long long int byteOffset = bytesHeader;
    indexData *arr = NULL;
    int lenArrIndex = 0;


    while(!feof(input)) {
        Data *reg = readBinaryRegister(input);

        if(isIntegerMember(memberName)) {
            if(strncmp(memberName, "idCrime", 7) == 0) {
                // checks not NULL
                // writes byteOffSet 
                // writes reg->idCrime
            }
            else if(strncmp(memberName, "numeroArtigo", 12) == 0) {
                // writes reg->articleNumber
                //arr = realloc
                //arr->byteoff = byteOffset
                //arr->searchKeyInt = reg->numartigo
            }
        }
        else {
            if (strncmp(memberName, "lugarCrime", 10) == 0) {
                // writes reg->crimePlace
            }
            else if (strncmp(memberName, "descricaoCrime", 14) == 0) {
            
            }
            else if (strncmp(memberName, "marcaCelular", 12) == 0) {
            
            }
            else if (strncmp(memberName, "dataCrime", 9) == 0) {
            
            }
        }

        // updating byteOffSet
        byteOffset += (stringLenght(reg->crimePlace) + stringLenght(reg->crimeDescription) + 2);
    }


}
