#include "search.h"
#include "registers.h"

struct result {
    int index;
    long long int *arrByteOff;
    int length;
    int found;
};

struct search {
    char *memberName;
    union {
        char *strMember;
        int intMember;
    };
    int len;
};

int getFoundFlag(Result *r) {
    return r->found;
}

void setFoundFlag(Result *r, int value) {
    r->found = value;
}

/*
* Function that creates a search array, that contains the name of a member
* from a register and its key
* It returns the adress of the array created
*/
Search *createSearchArr() {
    int arrLen;
    fscanf(stdin, "%d", &arrLen); //the number of searches/size of search array

    Search *tmp = (Search *)malloc(sizeof(Search) * arrLen);
    if (tmp == NULL) return NULL;

    getc(stdin); // reading garbage from stdin

    for (int i = 0; i < arrLen; i++) {
        //reading the member name
        tmp[i].memberName = readMember(stdin, ' '); 

        // if its an int type member, we got to convert to integer
        if (isIntegerMember(tmp[i].memberName)) {
            char *aux = readMember(stdin, ' ');
            tmp[i].intMember = atoi(aux);
        } 
        else { // its a string type member, reads value as string
            char *strAux = (char *)malloc(sizeof(char) * 50);
            scan_quote_string(strAux);
            tmp[i].strMember = strAux;
            getc(stdin); // reading garbage from stdin
        }
    }

    // setting the len in the struct
    tmp->len = arrLen;
    return tmp;
}

/*
* Function that searches a register in a binary file sequntially
* It returns a result struct that contains an array of byteoffset
* and an array of pos in the index file
*/
Result *sequentialSearch(FILE *dataFile, Search *wanted, Header *h) {
    // starting the byteoffset from the final of the first register
    long long int byteOffset = bytesHeader;
    int lenArrByteOffset = 0;
    int numRequirements = wanted->len;
    
    Result *r = (Result *)malloc(sizeof(Result));
    r->arrByteOff = NULL;
    
    // going to the byteoffset after the header
    fseek(dataFile, bytesHeader, SEEK_SET);

    // iterating for every register in the file
    for (int i = 0; i < getNumFileRegisters(h); i++) {
        Data *aux = readBinaryRegister(dataFile);

        int requirements = 0;
        int bytesCurrentReg = bytesFixedMember;

        for (int i = 0; i < numRequirements; i++) {

            // comparing 
            if (isIntegerMember(wanted[i].memberName)) {
                requirements += intMemberCompare(wanted[i].memberName, wanted[i].intMember, aux);
            }
            else {
                requirements += strMemberCompare(wanted[i].memberName, wanted[i].strMember, aux);
            }
        }

        // updating the current offset to the end of register in the file
        bytesCurrentReg += (stringLenght(getDataCrimePlace(aux)) + stringLenght(getDataCrimeDescription(aux)) + 2);
        
        // found compatible register
        if (requirements == numRequirements && getDataRemoved(aux) == '0') { 
            lenArrByteOffset++;
            r->arrByteOff = (long long int *)realloc(r->arrByteOff, sizeof(long long int) * lenArrByteOffset);

            // adding the byteoffset of the current register in the array
            r->arrByteOff[lenArrByteOffset-1] = byteOffset;
        }

        // updating the offset of the file
        byteOffset += bytesCurrentReg;
    }
    
    // updating the lenght of the result
    r->length = lenArrByteOffset;
    return r; 
}

Result *createResult() {
    Result *r = (Result *)malloc(sizeof(Result));

    r->arrByteOff = NULL;
    r->length = 0;
    r->index = -1;
    r->found = 0;

    return r;
}

Result *verifyingRegRequirements(FILE *dataFile, Result *resArr, Search *wanted) {
    int newLenArr = 0;
    int numRegFound = resArr->length;
    long long int *newArrByteOff = NULL;
    int numRequirements = wanted->len;

    // looping trough registers found and verifying its content
    for (int i = 0; i < numRegFound; i++) {
        // variable created to count the requirements a register fuffils
        int requirementsFufilled = 0;
        
        // fseeking to the register found in index tree file
        fseek(dataFile, resArr->arrByteOff[i], SEEK_SET);
        // reading the register
        Data *reg = readBinaryRegister(dataFile);

        // checking if register was removed
        if(getDataRemoved(reg) == '1') continue;

        for (int j = 0; j < numRequirements; j++) {

            // checking if the register have all requirements
            if (isIntegerMember(wanted[j].memberName)) { // int type
                requirementsFufilled += intMemberCompare(wanted[j].memberName, wanted[j].intMember, reg);
            }
            else { //string type
                requirementsFufilled += strMemberCompare(wanted[j].memberName, wanted[j].strMember, reg);
            }
        }

        // if all requirements are met, adds the register to the new arrbyteoffset
        if (requirementsFufilled == numRequirements) {
            newLenArr++;
            newArrByteOff = (long long int *)realloc(newArrByteOff, sizeof(long long int) * newLenArr);
            newArrByteOff[newLenArr-1] = resArr->arrByteOff[i];
        }
    }

    // updating the result struct with the new len, arr byteoffset and arr pos
    resArr->length = newLenArr;
    resArr->arrByteOff = newArrByteOff;

    return resArr;
}

/*
* This function is used to append byteOffset into a byteOffset array with lenght len
*/
long long int *byteOffsetArrAppend(long long int *arr, int len, long long int byteOffset) {
    arr = (long long int *)realloc(arr, sizeof(long long int) * len);
    arr[len-1] = byteOffset;

    return arr;
}


void appendResult(Result *r, long long int byteOff, int index) {
    r->length++;
    r->arrByteOff = byteOffsetArrAppend(r->arrByteOff, r->length, byteOff);
    r->index = index;
}

int getIntegerSearchValue(Search *s) {
    return s == NULL ? -1 : s->intMember;
}

int isMemberInIndex(Search *wanted, int iteration, char *memberNameIndex) {
    return (strcmp(wanted[iteration].memberName, memberNameIndex) == 0);
}

int searchingCrimeId (Search *wanted, int *key) {
    //check if we are searching idCrime
    //printf("entrou\n");
    int i = 0;
    while (i < wanted->len) {
        if (strncmp(wanted[i].memberName, "idCrime", 7) == 0) {
            *key = wanted[i].intMember;
            //printf("saiu\n");
            return 1;
        }

        i++;
    }
    return 0;
}

void printResultData(Result *res, FILE *dataFile) {   
    // printing result data in search type format
    if (res == NULL || res->length == 0) {
        printf("Registro inexistente.\n");
    }
    else { //arrByteOffset != NULL
        for (int j = 0; j < res->length; j++) {
            fseek(dataFile, res->arrByteOff[j], SEEK_SET);
            Data *d = readBinaryRegister(dataFile);
            printData(d);
        }
    }
}

long long int getByteoffset(Result *r, int idx) {
    return r->arrByteOff[idx];
}

int getIndex(Result *r) {
    return r->index;
}

int setIndex(Result *r, int index) {
    r->index = index;
}