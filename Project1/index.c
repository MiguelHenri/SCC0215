#include "index.h"
#include "registers.h"

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

IndexHeader *createIndexHeader() {
    IndexHeader *h = (IndexHeader *)malloc(sizeof(IndexHeader));

    h->numberOfRegisters -1;
    h->status = '0';

    return h;
}

// input == binary data file
IndexData *createIndexArr(FILE *input, IndexHeader *h, char *indexType, char *memberName) {

    long long int byteOffset = bytesHeader;
    IndexData *arr = NULL;
    int lenArrIndex = 0;
    Header *headerFile = readHeaderBinary(input);

    for (int i = 0; i < getNumFileRegisters(headerFile); i++) {
        Data *reg = readBinaryRegister(input);

        int currentOffset = byteOffset;

        // updating byteOffSet
        //printf("crime place: %s || len: %d\n",getDataCrimePlace(reg), stringLenght(getDataCrimePlace(reg)));
        byteOffset += (stringLenght(getDataCrimePlace(reg)) + stringLenght(getDataCrimeDescription(reg)) + 34);

        if (getDataRemoved(reg) == '1') continue;

        if(isIntegerMember(memberName)) { // int type
            if(strncmp(memberName, "idCrime", 7) == 0) {
                if (getDataCrimeId(reg) != -1) { 
                    // if not null, we add it to our array
                    arr = (IndexData *)realloc(arr, sizeof(IndexData) * (++lenArrIndex));
                    arr[lenArrIndex-1].byteOffset = currentOffset;
                    arr[lenArrIndex-1].searchKeyInt = getDataCrimeId(reg);
                }

            }
            else if(strncmp(memberName, "numeroArtigo", 12) == 0) {

                if (getDataArticleNumber(reg) != -1) { 
                    // if not null, we add it to our array
                    arr = (IndexData *)realloc(arr, sizeof(IndexData) * (++lenArrIndex));
                    arr[lenArrIndex-1].byteOffset = currentOffset;
                    arr[lenArrIndex-1].searchKeyInt = getDataArticleNumber(reg);
                }
                
            }
        }
        else { // string type
            if (strncmp(memberName, "lugarCrime", 10) == 0) {
                if (!regMissingData(reg) && getDataCrimePlace(reg) != NULL) { 
                    // if not null, we add it to our array
                    arr = (IndexData *)realloc(arr, sizeof(IndexData) * (++lenArrIndex));
                    arr[lenArrIndex-1].byteOffset = currentOffset;
                    arr[lenArrIndex-1].searchKeyStr = superStringCopy(getDataCrimePlace(reg), maxLenStr);
                }
            }
            else if (strncmp(memberName, "descricaoCrime", 14) == 0) {
                if (!regMissingData(reg) && getDataCrimeDescription(reg) != NULL) { 
                    // if not null, we add it to our array
                    arr = (IndexData *)realloc(arr, sizeof(IndexData) * (++lenArrIndex));
                    arr[lenArrIndex-1].byteOffset = currentOffset;
                    arr[lenArrIndex-1].searchKeyStr = superStringCopy(getDataCrimeDescription(reg), maxLenStr);
                }
            }
            else if (strncmp(memberName, "marcaCelular", 12) == 0) {
                if (!regMissingData(reg) && getDataTelephoneBrand(reg) != NULL) { 
                    // if not null, we add it to our array
                    arr = (IndexData *)realloc(arr, sizeof(IndexData) * (++lenArrIndex));
                    arr[lenArrIndex-1].byteOffset = currentOffset;
                    arr[lenArrIndex-1].searchKeyStr = superStringCopy(getDataTelephoneBrand(reg), maxLenStr);
                }
            }
            else if (strncmp(memberName, "dataCrime", 9) == 0) {
                if (!regMissingData(reg) && getDataCrimeDate(reg) != NULL) { 
                    // if not null, we add it to our array
                    arr = (IndexData *)realloc(arr, sizeof(IndexData) * (++lenArrIndex));
                    arr[lenArrIndex-1].byteOffset = currentOffset;
                    arr[lenArrIndex-1].searchKeyStr = superStringCopy(getDataCrimeDate(reg), maxLenStr);
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
        }
    }

}

int countDate(char *dateStr) {
    int days = 0;
    
    char delimiter[] = "/";
    char *tokAux = NULL;

    tokAux = strtok(dateStr, delimiter);
    days += atoi(tokAux);

    tokAux = strtok(NULL, delimiter);
    days += atoi(tokAux) * 30;

    tokAux = strtok(NULL, "");
    days += atoi(tokAux) * 365;

    return days;
}

void sortIndexArrString(IndexData *arr, int len, int isDate) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len-i-1; j++) {
            if (isDate == 1) {
                int days0 = countDate(arr[j].searchKeyStr);
                int days1 = countDate(arr[j+1].searchKeyStr);
                if (days0 > days1) {
                    swap(arr, j, j+1);
                }
            }
            else if (strncmp(arr[j].searchKeyStr, arr[j+1].searchKeyStr, 12) > 0) {
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
            fwrite (&(arr[i].byteOffset), sizeof(long long int), 1, index);
            fwrite(&(arr[i].searchKeyInt), sizeof(int), 1, index);
        }
    }
    else {
        sortIndexArrString(arr, arrLen, strncmp(memberName, "dataCrime", 9) == 0);
        for (int i = 0; i < arrLen; i++) {
            for (int j = 0; j < 12; j++)
                fwrite(&(arr[i].searchKeyStr[j]), sizeof(char), 1, index);
            fwrite (&(arr[i].byteOffset), sizeof(long long int), 1, index);
        }
    }
} 

IndexData *readFileIndex(FILE *indexFile, char *memberName, IndexHeader *header) {
    char charAux;
    int intAux;
    long long int llintAux;

    IndexData *array = (IndexData *)malloc(sizeof(IndexData) * header->numberOfRegisters);

    for (int i = 0; i < header->numberOfRegisters; i++) {
        // reading data
        if (isIntegerMember(memberName)) {
            fread(&intAux, sizeof(int), 1, indexFile);
            array[i].searchKeyInt = intAux;
        }
        else {
            char *strAux = (char *)malloc(sizeof(char) * maxLenStr);
            for (int j = 0; j < maxLenStr; j++)
                fread(&strAux[j], sizeof(char), 1, indexFile);

            array[i].searchKeyStr = strAux;
        }

        // reading byteOffset
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

long long int *searchInIndexArr(IndexData *arr, IndexHeader *h, Search *wanted, int iteration, char *memberName, int *sizeArrByte) {
    int lenArrByteOff = 0;
    long long int *arrByteOff = NULL;
    
    int begin = 0;
    int end = h->numberOfRegisters - 1;

    while (end <= begin) {
        int mid = (begin + end) / 2;
        if (isIntegerMember(memberName)) {
            if (arr[mid].searchKeyInt == getSearchIntKey(wanted, iteration)) { // found
                arrByteOff = (long long int *)realloc(arrByteOff, sizeof(long long int) * (++lenArrByteOff));
                arrByteOff[lenArrByteOff-1] = arr[mid].byteOffset;
                int midAux = mid;

                //going to the end of the array searching for others registers
                while (arr[++mid].searchKeyInt == getSearchIntKey(wanted, iteration)) {
                    arrByteOff = (long long int *)realloc(arrByteOff, sizeof(long long int) * (++lenArrByteOff));
                    arrByteOff[lenArrByteOff-1] = arr[mid].byteOffset;
                }
                while (arr[--mid].searchKeyInt == getSearchIntKey(wanted, iteration)) {
                    arrByteOff = (long long int *)realloc(arrByteOff, sizeof(long long int) * (++lenArrByteOff));
                    arrByteOff[lenArrByteOff-1] = arr[mid].byteOffset;
                }
            }
            else { // keep searching
                if (arr[mid].searchKeyInt > getSearchIntKey(wanted, iteration)) end = mid-1;
                else begin = mid+1;
            }
        }
        else { //looking for a string in index array
            if (strcmp(arr[mid].searchKeyStr, getSearchStrKey(wanted, iteration)) == 0) { // found
                arrByteOff = (long long int *)realloc(arrByteOff, sizeof(long long int) * (++lenArrByteOff));
                arrByteOff[lenArrByteOff-1] = arr[mid].byteOffset;
                int midAux = mid;

                while (strcmp(arr[++mid].searchKeyStr, getSearchStrKey(wanted, iteration)) == 0) {
                    arrByteOff = (long long int *)realloc(arrByteOff, sizeof(long long int) * (++lenArrByteOff));
                    arrByteOff[lenArrByteOff-1] = arr[mid].byteOffset;
                }

                while (strcmp(arr[--midAux].searchKeyStr, getSearchStrKey(wanted, iteration)) == 0) {
                    arrByteOff = (long long int *)realloc(arrByteOff, sizeof(long long int) * (++lenArrByteOff));
                    arrByteOff[lenArrByteOff-1] = arr[mid].byteOffset;
                }
            }
            else {
                if (strcmp(arr[mid].searchKeyStr, getSearchStrKey(wanted, iteration)) > 0) end = mid-1;
                else begin = mid+1;
            }
        
        }
    
    }

    *sizeArrByte = lenArrByteOff;
    return arrByteOff;
}
