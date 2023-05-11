#include "index.h"
#include "registers.h"
#include "utils.h"

struct indexHeader {
    char status;
    int numberOfRegisters;
};

struct indexData {
    long long int byteOffset;
    union {
        char *searchKeyStr;
        int searchKeyInt;
    };
};

struct result {
    long long int *arrByteOff;
    int *arrPos;
    int length;
};

struct search {
    char *memberName;
    union {
        char *strMember;
        int intMember;
    };
};

IndexHeader *createIndexHeader() {
    IndexHeader *h = (IndexHeader *)malloc(sizeof(IndexHeader));

    h->numberOfRegisters -1;
    h->status = '0';

    return h;
}

void readIndexHeader(FILE *indexFile, IndexHeader *indexHeader) {
    if (indexHeader == NULL) return;

    char charAux; int intAux;

    fread(&charAux, sizeof(char), 1, indexFile);
    setIndexHeaderStatus(indexHeader, charAux);

    fread(&intAux, sizeof(int), 1, indexFile);
    setIndexHeaderNumReg(indexHeader, intAux);
}

IndexData *indexArrayAppend(IndexData *arr, int *len, int intKey, char *strKey, long long int byteoffset) {
    //dont append because this register have null values
    if (intKey == -1 && strKey == NULL) 
        return arr;
        
    arr = (IndexData *)realloc(arr, sizeof(IndexData) * (++(*len)));

    if (strKey != NULL) { //appending a string in the index array
        arr[*len - 1].searchKeyStr = strKey;
    }
    else { //appendig an int in the index array
        arr[*len - 1].searchKeyInt = intKey;
    }

    arr[*len - 1].byteOffset = byteoffset;

    return arr;
}

void printIndexHeader(IndexHeader *h) {
    printf("tem %d registros no indice\n", h->numberOfRegisters);
}


IndexData *appendIndexArray(IndexData *arr, IndexHeader *h, char *memberName, int intKey, char *strKey, long long int byteOff)  {
    //dont append because this register have null values
    if (intKey == -1 && strKey == NULL) 
        return arr;

    int newLen = h->numberOfRegisters + 1;


    if (isIntegerMember(memberName)) {
        if (intKey == -1) return arr;

        arr = (IndexData *)realloc(arr, sizeof(IndexData) * newLen);
        arr[newLen-1].searchKeyInt = intKey;
        // printf("inseri %lld no vetor\n", arr[newLen-1].byteOffset);
    }
    else {
        if (strKey == NULL) return arr; 

        arr = (IndexData *)realloc(arr, sizeof(IndexData) * newLen);
        arr[newLen-1].searchKeyStr = strKey;
    }

    arr[newLen-1].byteOffset = byteOff;
    h->numberOfRegisters++;

    return arr;
}

// input == binary data file
IndexData *createIndexArr(FILE *input, IndexHeader *h, char *indexType, char *memberName) {
    Header *headerFile = readHeaderBinary(input);
    if (!verifyFileIntegrity(headerFile)) { //verifying if the file is usable
        FILE_ERROR;
        exit(0);
    }
    
    long long int byteOffset = bytesHeader;
    IndexData *arr = NULL;
    int lenArrIndex = 0;

    for (int i = 0; i < getNumFileRegisters(headerFile); i++) {
        Data *reg = readBinaryRegister(input);

        //auxiliary variable to count the current offset that will be used in the array
        int currentOffset = byteOffset; 

        // updating byteOffSet
        byteOffset += (stringLenght(getDataCrimePlace(reg)) + stringLenght(getDataCrimeDescription(reg)) + 34);

        //register removed, we go to the next one
        if (getDataRemoved(reg) == '1') continue;

        if(isIntegerMember(memberName)) { // int type
            if(strncmp(memberName, "idCrime", 7) == 0) {
                if (getDataCrimeId(reg) != -1) { 
                    // if not null, we add it to our array

                    arr = indexArrayAppend(arr, &lenArrIndex, getDataCrimeId(reg), NULL, currentOffset);
                }

            }
            else if(strncmp(memberName, "numeroArtigo", 12) == 0) {

                if (getDataArticleNumber(reg) != -1) { 
                    // if not null, we add it to our array
                    
                    arr = indexArrayAppend(arr, &lenArrIndex, getDataArticleNumber(reg), NULL, currentOffset);
                }
                
            }
        }
        else { // string type
            if (strncmp(memberName, "lugarCrime", 10) == 0) {
                if (!regMissingData(reg) && getDataCrimePlace(reg) != NULL) { 
                    // if not null, we add it to our array
                    
                    arr = indexArrayAppend(arr, &lenArrIndex, -1, superStringCopy(getDataCrimePlace(reg), maxLenStr), currentOffset);
                }
            }
            else if (strncmp(memberName, "descricaoCrime", 14) == 0) {
                if (!regMissingData(reg) && getDataCrimeDescription(reg) != NULL) { 
                    // if not null, we add it to our array
                    
                    arr = indexArrayAppend(arr, &lenArrIndex, -1, superStringCopy(getDataCrimeDescription(reg), maxLenStr), currentOffset);

                }
            }
            else if (strncmp(memberName, "marcaCelular", 12) == 0) {
                if (!regMissingData(reg) && getDataTelephoneBrand(reg) != NULL) { 
                    // if not null, we add it to our array
                    
                    arr = indexArrayAppend(arr, &lenArrIndex, -1, superStringCopy(getDataTelephoneBrand(reg), maxLenStr), currentOffset);
                }
            }
            else if (strncmp(memberName, "dataCrime", 9) == 0) {
                if (!regMissingData(reg) && getDataCrimeDate(reg) != NULL) { 
                    // if not null, we add it to our array
                    
                    arr = indexArrayAppend(arr, &lenArrIndex, -1, superStringCopy(getDataCrimeDate(reg), maxLenStr), currentOffset);
                }
            }
        }

    }

    h->numberOfRegisters = lenArrIndex;
    return arr;
}

void swap(IndexData *arr, int index1, int index2) {
    IndexData t = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = t;
}

void sortIndexArrInt(IndexData *arr, int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len-i-1; j++) {
            if (arr[j].searchKeyInt > arr[j+1].searchKeyInt) {
                swap(arr, j, j+1);
            }
            if (arr[j].searchKeyInt == arr[j+1].searchKeyInt &&
                arr[j].byteOffset > arr[j+1].byteOffset) {
                swap(arr, j, j+1);
            }
        }
    }

}


void sortIndexArrString(IndexData *arr, int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len-i-1; j++) {
            if (strncmp(arr[j].searchKeyStr, arr[j+1].searchKeyStr, 12) > 0) {
                swap(arr, j, j+1);
            }
            
        }
    }
}

void writeFileIndex(FILE *index, IndexData *arr, IndexHeader *h, char *memberName) {
    if (arr == NULL || h == NULL) return;

    int arrLen = h->numberOfRegisters;
    h->status = '1';
    fwrite(&(h->status), sizeof(char), 1, index);
    fwrite(&(h->numberOfRegisters), sizeof(int), 1, index);


    if (isIntegerMember(memberName)) {
        sortIndexArrInt(arr, arrLen);

        for (int i = 0; i < arrLen; i++) {
            
            //writing the index data 
            fwrite(&(arr[i].searchKeyInt), sizeof(int), 1, index);
            
            //writing the byteoffset
            fwrite (&(arr[i].byteOffset), sizeof(long long int), 1, index);
        }
    }
    else {
        sortIndexArrString(arr, arrLen);

        for (int i = 0; i < arrLen; i++) {

            //writing the string byte per byte
            for (int j = 0; j < 12; j++)
                fwrite(&(arr[i].searchKeyStr[j]), sizeof(char), 1, index);

            //writing the byteoffset
            fwrite (&(arr[i].byteOffset), sizeof(long long int), 1, index);
        }
    }
} 

IndexData *readFileIndex(FILE *indexFile, char *memberName, IndexHeader *header) {
    char charAux;
    int intAux;
    long long int llintAux;

    // alocatting the array of our index data
    IndexData *array = (IndexData *)malloc(sizeof(IndexData) * header->numberOfRegisters);
    

    // reading data
    for (int i = 0; i < header->numberOfRegisters; i++) {

        //verifying if we will have to read a integer or string
        if (isIntegerMember(memberName)) {
            fread(&intAux, sizeof(int), 1, indexFile);

            // inserting in the array
            array[i].searchKeyInt = intAux;
        }
        else {
            char *strAux = (char *)malloc(sizeof(char) * maxLenStr);

            // reading the string byte per byte
            for (int j = 0; j < maxLenStr; j++)
                fread(&strAux[j], sizeof(char), 1, indexFile);

            // inserting in the array
            array[i].searchKeyStr = strAux;
        }

        // reading byteOffset and inserting it
        fread(&llintAux, sizeof(long long int), 1, indexFile);
        array[i].byteOffset = llintAux;
    }
    
    return array;
}

void setIndexHeaderStatus(IndexHeader *h, char status) {
    if (h == NULL) return;
    h->status = status;
}

void setIndexHeaderNumReg(IndexHeader *h, int numReg) {
    if (h == NULL) return;
    h->numberOfRegisters = numReg; 
}

int *posArrAppend(int *arr, int len, int pos) {
    arr = (int *)realloc(arr, sizeof(int) * len);
    arr[len-1] = pos; 

    return arr;
}


Result *searchInIndexArr(IndexData *arr, IndexHeader *h, Search *wanted, int iteration, char *memberName) {
    int lenArrByteOff = 0;
    Result *r = (Result *)malloc(sizeof(Result));
    r->arrByteOff = NULL;
    r->arrPos = NULL;
    r->length = 0;
    
    int begin = 0;
    int end = h->numberOfRegisters - 1;

    //printIndex(arr, h);

    while (end > begin) {
        int mid = (begin + end) / 2;

        if (isIntegerMember(memberName)) {
            if (arr[mid].searchKeyInt == wanted[iteration].intMember) { // found
                // printf("encontrei %d\n");
                int verifyPrevious = mid;
                while (arr[--verifyPrevious].searchKeyInt == wanted[iteration].intMember) 

                mid = verifyPrevious + 1;
                while (arr[mid].searchKeyInt == wanted[iteration].intMember) {
                    r->arrByteOff = byteOffsetArrAppend(r->arrByteOff, ++lenArrByteOff, arr[mid].byteOffset);
                    r->arrPos = posArrAppend(r->arrPos, lenArrByteOff, mid);
                    mid++;
                }
            
                break;
            }
            else { // keep searching
                if (arr[mid].searchKeyInt > wanted[iteration].intMember) end = mid;
                else begin = mid;
            }
        }
        else { //looking for a string in index array
            // printf("na pos %d tem %s\n", mid, getSearchStrKey(wanted, iteration));
            int cmpResult = strncmp(arr[mid].searchKeyStr, superStringCopy(wanted[iteration].strMember, maxLenStr), maxLenStr);
            if (cmpResult == 0) { // found
    
                int midAux = mid;

                r->arrByteOff = byteOffsetArrAppend(r->arrByteOff, ++lenArrByteOff, arr[mid].byteOffset);
                r->arrPos = posArrAppend(r->arrPos, lenArrByteOff, mid);


                while (strncmp(arr[--midAux].searchKeyStr, superStringCopy(wanted[iteration].strMember, maxLenStr), maxLenStr) == 0) {
                    r->arrByteOff = byteOffsetArrAppend(r->arrByteOff, ++lenArrByteOff, arr[midAux].byteOffset);
                    r->arrPos = posArrAppend(r->arrPos, lenArrByteOff, midAux);

                }
                while (strncmp(arr[++mid].searchKeyStr, superStringCopy(wanted[iteration].strMember, maxLenStr), 12) == 0) {
                    r->arrByteOff = byteOffsetArrAppend(r->arrByteOff, ++lenArrByteOff, arr[mid].byteOffset);
                    r->arrPos = posArrAppend(r->arrPos, lenArrByteOff, mid);

                }



                break;
            }
            else {
                if (cmpResult > 0) end = mid;
                else begin = mid+1;
            }
        
        }
    
    }

    r->length = lenArrByteOff;
    return r;
}

Result *verifyingRegRequirements(FILE *input, Result *indexResArray, Search *wanted, int numRequirements) {

    int newLenArr = 0;
    long long int *newArrByteOff = NULL;
    int *newArrPos = NULL;
    int lenAux = indexResArray->length;

    //printf("\n [verifying] \n", lenAux);

    for (int i=0; i < lenAux; i++) {
        int requirementsFufilled = 0;
        
        // fseek para o registro encontrado no arquivo de dados
        fseek(input, indexResArray->arrByteOff[i], SEEK_SET);
        // lendo o registro
        Data *reg = readBinaryRegister(input);

        for (int j = 0; j < numRequirements; j++) {

            // verificando se o registro esta dentro dos parametros de busca
            if (!isIntegerMember(wanted[j].memberName)) { // string type
                requirementsFufilled += strMemberCompare(wanted[j].memberName, wanted[j].strMember, reg);
            }
            else { // int type
                requirementsFufilled += intMemberCompare(wanted[j].memberName, wanted[j].intMember, reg);
            }
        }

        //se todos requisitos foram cumpridos, adiciona no vetor final de byteoffset
        if (requirementsFufilled == numRequirements) {
            newLenArr++;
            newArrByteOff = (long long int *)realloc(newArrByteOff, sizeof(long long int) * newLenArr);
            newArrPos = (int *)realloc(newArrPos, sizeof(int) * newLenArr);
            
            newArrByteOff[newLenArr-1] = indexResArray->arrByteOff[i];
            newArrPos[newLenArr-1] = indexResArray->arrPos[i];

            // printf("inserindo o byte %lld\n", );
        }
    }

    indexResArray->length = newLenArr;
    indexResArray->arrByteOff = newArrByteOff;
    indexResArray->arrPos = newArrPos;
    return indexResArray;
}

int isMemberInIndex(Search *wanted, int iteration, char *memberNameIndex) {
    return (strcmp(wanted[iteration].memberName, memberNameIndex) == 0);
}


Search *createSearchArr(FILE *input, int *numberPairs) {
    int arrLen;
    fscanf(stdin, "%d", &arrLen); //the number of searches/size of search array

    Search *tmp = (Search *)malloc(sizeof(Search) * arrLen);
    if (tmp == NULL) return NULL;

    for (int i = 0; i < arrLen; i++) {
        getc(stdin);
        tmp[i].memberName = readMember(stdin, ' '); //reading the member name
        
        // its an int type member, reads value using %d
        if (isIntegerMember(tmp[i].memberName)) {
            int aux;
            fscanf(stdin, "%d", &aux);
            tmp[i].intMember = aux;
        } 
        else { // its a string type member, reads value as string
            char *strAux = (char *)malloc(sizeof(char) * 50);
            scan_quote_string(strAux);
            tmp[i].strMember = strAux;
        }
    }

    *numberPairs = arrLen;
    return tmp;
}

Result *sequentialSearch(FILE *input, Search *wanted, int numberPairs) {
    if (!readHeaderBinary(input)) return NULL;
    
    long long int byteOffset = bytesHeader;
    int lenArrByteOffset = 0;
    
    Result *r = (Result *)malloc(sizeof(Result));
    r->arrByteOff = NULL;
    r->arrPos = NULL;

    while (!feof(input)) {
        Data *aux = readBinaryRegister(input);

        int requirements = 0;
        int bytesCurrentReg = bytesFixedMember;

        for (int i = 0; i < numberPairs; i++) {

            if (!isIntegerMember(wanted[i].memberName)) {
                requirements += strMemberCompare(wanted[i].memberName, wanted[i].strMember, aux);
            }
            else {
                requirements += intMemberCompare(wanted[i].memberName, wanted[i].intMember, aux);
            }

        }

        bytesCurrentReg += (stringLenght(getDataCrimePlace(aux)) + stringLenght(getDataCrimeDescription(aux)) + 2);
        
        if (requirements == numberPairs && getDataRemoved(aux) == '0') { //achou um registro compativel
            lenArrByteOffset++;
            r->arrByteOff = (long long int *)realloc(r->arrByteOff, sizeof(long long int) * lenArrByteOffset);

            //adding the byteoffset of the current register in the array
            r->arrByteOff[lenArrByteOffset-1] = byteOffset;
        }

        //adding the size of the current register in the file byteoffset counter
        byteOffset += bytesCurrentReg;
    }
    
    r->length = lenArrByteOffset;
    return r; 
}


void printIndex(IndexData *arr, IndexHeader *h) {
    if (arr == NULL) {
        printf("arr nulo\n");
        return;
    }

    for (int i = 0; i < h->numberOfRegisters; i++) {
        printf("Valor: %d || byteoff: %lld\n", arr[i].searchKeyInt, arr[i].byteOffset);
    }

}

Result *superSearch(FILE *input, char *memberName, IndexData *arrIndex, IndexHeader *header, Search *s, int pairs) {
    // variable that stores byte offset array length

    //check if will be search by index or linear search
    Result *indexSearchResult = NULL;
    if (isMemberInIndex(s, 0, memberName)) {
        // fazendo a busca binaria no arquivo de indice
        indexSearchResult = searchInIndexArr(arrIndex, header, s, 0, memberName);

        // checking registers found by index due to other search requirements
        indexSearchResult = verifyingRegRequirements(input, indexSearchResult, s, pairs);
    }
    else {
        indexSearchResult = sequentialSearch(input, s, pairs);
    }
    
    return indexSearchResult;
}

int getResLenght(Result *r) {
    return r->length;
}

long long int getResByteOffset(Result *r, int pos) {
    return r->arrByteOff[pos];
}

int getIndexArrLen(IndexHeader *h) {
    return h->numberOfRegisters;
}

void superDelete(FILE *input, FILE *index, Result *toDelete, IndexData *indexDataArr, IndexHeader *indexHd, char *memberName) {
    // number of deletions
    int lenAux = toDelete->length;

    // sorting positions array
    sortIntArr(toDelete->arrPos, lenAux);

    for(int i=0; i<lenAux; i++) {
        printf("[%d]", toDelete->arrPos[i]);
    }

    // running toDelete vector and deleting data
    for(int i=0; i<lenAux; i++) {
        // deleting in input (data) file 
        // fseek to register position in input (data) file
        long long int byteOff = toDelete->arrByteOff[i];
        fseek(input, byteOff, SEEK_SET);

        // overwrites data->removed to '1'
        char removed = '1';
        fwrite(&removed, sizeof(char), 1, input);

        // deleting in index file
        int position = toDelete->arrPos[i] - i; // position to be deleted
        for(int i=position; i<indexHd->numberOfRegisters; i++) {

        }

        // subtracts register removed
        indexHd->numberOfRegisters = indexHd->numberOfRegisters - 1;
    }

    //writeFileIndex(index, indexDataArr, indexHd, )
}

