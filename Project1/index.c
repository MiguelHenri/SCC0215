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

// input == binary data file
IndexData *createIndexArr(FILE *input, IndexHeader *h, char *indexType, char *memberName) {

    long long int byteOffset = bytesHeader;
    IndexData *arr = NULL;
    int lenArrIndex = 0;


    while(!feof(input)) {
        Data *reg = readBinaryRegister(input);

        int currentOffset = byteOffset;

        // updating byteOffSet
        byteOffset += (stringLenght(getDataCrimePlace(reg)) + stringLenght(getDataCrimeDescription(reg)) + 2);

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
            fwrite (&(arr[i].byteOffset), sizeof(long long int), 1, index);
            fwrite(&(arr[i].searchKeyStr), sizeof(char) * 12, 1, index);
        }
    }

} 
