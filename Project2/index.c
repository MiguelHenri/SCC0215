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

/*
* Functiona that creates a index header struct in memory
* It returns the adress of the struct created
*/
IndexHeader *createIndexHeader() {
    IndexHeader *h = (IndexHeader *)malloc(sizeof(IndexHeader));

    h->numberOfRegisters -1;
    h->status = '0';

    return h;
}

/*
* Function that reads from a index file the reader and sets the
* members of the struct based on the data read
*/
void readIndexHeader(FILE *indexFile, IndexHeader *indexHeader) {
    if (indexHeader == NULL || indexFile == NULL) return;
    char charAux; int intAux;

    // reading the status of the file
    fread(&charAux, sizeof(char), 1, indexFile);

    if(charAux == '0') { // inconsistent file
        FILE_ERROR;
        exit(0);
    }

    setIndexHeaderStatus(indexHeader, charAux);

    // reading number os registers in the index file
    fread(&intAux, sizeof(int), 1, indexFile);
    setIndexHeaderNumReg(indexHeader, intAux);
}

void printIndexHeader(IndexHeader *h) {
    printf("tem %d registros no indice\n", h->numberOfRegisters);
}

/*
* Function that appends a key and its byteoffset in the index data array,
* the value in the index header is also updated
* It returns the adress of the new array realloced
*/
IndexData *appendIndexArray(IndexData *arr, IndexHeader *h, char *memberName, int intKey, char *strKey, long long int byteOff)  {
    //dont append because this register have null values
    if (intKey == -1 && strKey == NULL) 
        return arr;

    int newLen = h->numberOfRegisters + 1;

    // vrifying the data type we are dealing with
    if (isIntegerMember(memberName)) { // dealing with integers

        // null value, so we dont append
        if (intKey == -1) return arr; 

        // allocating more memory and inserting the key in the array
        arr = (IndexData *)realloc(arr, sizeof(IndexData) * newLen);
        arr[newLen-1].searchKeyInt = intKey;
    }
    else {// dealing with strings

        // verifying null values
        if (strKey == NULL) return arr; 
        if (strKey[0] == '$') return arr;

        // allocating more memory and inserting the key in the array
        arr = (IndexData *)realloc(arr, sizeof(IndexData) * newLen);
        arr[newLen-1].searchKeyStr = superStringCopy(strKey, maxLenStr);
    }

    // inserting the byteoffset in the position
    arr[newLen-1].byteOffset = byteOff;
    // increasing the amount of registers in the header struct
    h->numberOfRegisters++;

    return arr; // returning the new adress
}

/*
* Functions that creates and indexData array with data from the file 'input'
*/
IndexData *createIndexArr(FILE *input, IndexHeader *h, char *memberName) {
    Header *headerFile = readHeaderBinary(input);
    if (!verifyFileIntegrity(headerFile)) { //verifying if the file is usable
        FILE_ERROR;
        exit(0);
    }
    
    // starting the byteoffset after the header
    long long int byteOffset = bytesHeader;
    IndexData *arr = NULL;
    int lenArrIndex = 0;

    // reading all registers from the binary file
    for (int i = 0; i < getNumFileRegisters(headerFile); i++) {
        Data *reg = readBinaryRegister(input);

        //auxiliary variable to count the current offset that will be used in the array
        int currentOffset = byteOffset; 

        // updating byteOffSet
        byteOffset += (stringLenght(getDataCrimePlace(reg)) + stringLenght(getDataCrimeDescription(reg)) + 34);

        //register removed, we go to the next one
        if (getDataRemoved(reg) == '1') continue;

        // we have to know if we are dealing with strings or integers
        if(isIntegerMember(memberName)) { 
            if(strncmp(memberName, "idCrime", 7) == 0) {
                // if not null, we add it to our array
                if (getDataCrimeId(reg) != -1) { 
                    arr = appendIndexArray(arr, h, memberName, getDataCrimeId(reg), NULL, currentOffset);
                }
            }
            else if(strncmp(memberName, "numeroArtigo", 12) == 0) {
                // if not null, we add it to our array
                if (getDataArticleNumber(reg) != -1) { 
                    arr = appendIndexArray(arr, h, memberName, getDataArticleNumber(reg), NULL, currentOffset);
                }
            }
        }
        else { // string type
            if (strncmp(memberName, "lugarCrime", 10) == 0) {
                // if not null, we add it to our array
                if (!regMissingData(reg) && getDataCrimePlace(reg) != NULL) { 
                    arr = appendIndexArray(arr, h, memberName, -1, superStringCopy(getDataCrimePlace(reg), maxLenStr), currentOffset);  
                }
            }
            else if (strncmp(memberName, "descricaoCrime", 14) == 0) {
                // if not null, we add it to our array
                if (!regMissingData(reg) && getDataCrimeDescription(reg) != NULL) {     
                    arr = appendIndexArray(arr, h, memberName, -1, superStringCopy(getDataCrimeDescription(reg), maxLenStr), currentOffset);
                }
            }
            else if (strncmp(memberName, "marcaCelular", 12) == 0) {
                // if not null, we add it to our array
                if (!regMissingData(reg) && getDataTelephoneBrand(reg) != NULL) { 
                    arr = appendIndexArray(arr, h, memberName, -1, superStringCopy(getDataTelephoneBrand(reg), maxLenStr), currentOffset);
                }
            }
            else if (strncmp(memberName, "dataCrime", 9) == 0) {
                // if not null, we add it to our array
                if (!regMissingData(reg) && getDataCrimeDate(reg) != NULL) { 
                    arr = appendIndexArray(arr, h, memberName, -1, superStringCopy(getDataCrimeDate(reg), maxLenStr), currentOffset);
                }
            }
        }
    }

    return arr;
}

/*
* Function that swaps the positions in the index data array
*/
void swap(IndexData *arr, int index1, int index2) {
    IndexData t = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = t;
}

/*
* Functions that sorts the index data array with integers key
*/
void sortIndexArrInt(IndexData *arr, int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len-i-1; j++) {

            // swapping if current is greater than the next one
            if (arr[j].searchKeyInt > arr[j+1].searchKeyInt) {
                swap(arr, j, j+1);
            }
            // swapping if the keys are equal but the byteoffset of the current 
            // is greater than the next one
            if (arr[j].searchKeyInt == arr[j+1].searchKeyInt &&
                arr[j].byteOffset > arr[j+1].byteOffset) {
                swap(arr, j, j+1);
            }
        }
    }

}

/*
* Functions that sorts the index data array with string key
*/
void sortIndexArrString(IndexData *arr, int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len-i-1; j++) {

            // swapping if current is greater than the next one
            if (strncmp(arr[j].searchKeyStr, arr[j+1].searchKeyStr, 12) > 0) {
                swap(arr, j, j+1);
            }
            // swapping if the keys are equal but the byteoffset of the current 
            // is greater than the next one
            if (strncmp(arr[j].searchKeyStr, arr[j+1].searchKeyStr, 12) == 0 &&
                arr[j].byteOffset > arr[j+1].byteOffset) {
                swap(arr, j, j+1);
            }
            
        }
    }
}

/*
* Functions that writes a given index data array in a binary file
*/
void writeFileIndex(FILE *index, IndexData *arr, IndexHeader *h, char *memberName) {
    if (arr == NULL || h == NULL) return;

    // writing the inconsistent status in the file
    h->status = '0';
    fwrite(&(h->status), sizeof(char), 1, index);
    fwrite(&(h->numberOfRegisters), sizeof(int), 1, index);

    int arrLen = h->numberOfRegisters;
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

    // writing the consistent status in the file
    h->status = '1';
    fseek(index, 0, SEEK_SET);
    fwrite(&(h->status), sizeof(char), 1, index);
} 

/*
* Function that reads an index file and creates a index data array based on the file
* It returns the adress of the new array created
*/
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

            // reading and inserting in the array
            fread(&intAux, sizeof(int), 1, indexFile);
            array[i].searchKeyInt = intAux;
        }
        else {
            // alocating memory and reading the string byte per byte
            char *strAux = (char *)malloc(sizeof(char) * maxLenStr);
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

/*
* Function that alocates more memory and inserts a new pos in the array pos
*/
int *posArrAppend(int *arr, int len, int pos) {
    arr = (int *)realloc(arr, sizeof(int) * len);
    arr[len-1] = pos; 

    return arr;
}

/*
* Auxiliary function that finds all int keys in the binary search called
* in searchIndexArr 
* It returns the new lenght of the array 
*/
int findAllIntKeys(IndexData *arr, Result *r, Search *wanted, int posWanted, int currentIdx, int lenArrByteOff) {
    int verifyPrevious = currentIdx;
    while (verifyPrevious >= 0 && arr[verifyPrevious].searchKeyInt == wanted[posWanted].intMember)
        verifyPrevious -= 1;

    currentIdx = verifyPrevious + 1;
    while (arr[currentIdx].searchKeyInt == wanted[posWanted].intMember) {
        r->arrByteOff = byteOffsetArrAppend(r->arrByteOff, ++lenArrByteOff, arr[currentIdx].byteOffset);
        r->arrPos = posArrAppend(r->arrPos, lenArrByteOff, currentIdx);

        currentIdx++;
    }

    return lenArrByteOff;
}

int findAllStrKeys(IndexData *arr, Result *r, Search *wanted, int posWanted, int currentIdx, int lenArrByteOff) {
    int verifyPrevious = currentIdx;
    while (verifyPrevious >= 0 && 
           strncmp(arr[verifyPrevious].searchKeyStr, superStringCopy(wanted[posWanted].strMember, maxLenStr), maxLenStr) == 0)
        verifyPrevious -= 1;

    currentIdx = verifyPrevious + 1;
    while (strncmp(arr[currentIdx].searchKeyStr, superStringCopy(wanted[posWanted].strMember, maxLenStr), maxLenStr) == 0) {
        r->arrByteOff = byteOffsetArrAppend(r->arrByteOff, ++lenArrByteOff, arr[currentIdx].byteOffset);
        r->arrPos = posArrAppend(r->arrPos, lenArrByteOff, currentIdx);

        currentIdx++;
    }

    return lenArrByteOff;
}

Result *searchInIndexArr(IndexData *arr, IndexHeader *h, Search *wanted, int iteration, char *memberName) {
    int lenArrByteOff = 0;
    Result *r = (Result *)malloc(sizeof(Result));
    r->arrByteOff = NULL;
    r->arrPos = NULL;
    r->length = 0;
    
    int begin = 0;
    int end = h->numberOfRegisters - 1;

    while (end >= begin) {
        int mid = (begin + end) / 2;

        if (isIntegerMember(memberName)) { // looking for an int in index array
            
            // found
            if (arr[mid].searchKeyInt == wanted[iteration].intMember) { 
                lenArrByteOff = findAllIntKeys(arr, r, wanted, iteration, mid, lenArrByteOff);
                break;
            }
            // searching in the beggining of the subdivided array
            else if (arr[mid].searchKeyInt > wanted[iteration].intMember) {
                end = mid-1;
            }
            // searching in the ending of the subdivided array
            else {
                begin = mid+1;
            }
        }
        else { //looking for a string in index array
            int cmpResult = strncmp(arr[mid].searchKeyStr, superStringCopy(wanted[iteration].strMember, maxLenStr), maxLenStr);
            
            // found the key we want
            if (cmpResult == 0) { 
                lenArrByteOff = findAllStrKeys(arr, r, wanted, iteration, mid, lenArrByteOff);
                break;
            }
            // searching in the beggining of the subdivided array
            else if (cmpResult > 0) {
                end = mid-1;
            }
            // searching in the ending of the subdivided array
            else {
                begin = mid+1;
            }
        }
    }

    // updating the new len of the arrays in the result struct
    r->length = lenArrByteOff;
    return r;
}

/*
* Function created to verify if a register found in the binary search fufills
* all the requirements asked by the user
*/
Result *verifyingRegRequirements(FILE *input, Result *resArr, Search *wanted) {
    int newLenArr = 0;
    int lenAux = resArr->length;
    long long int *newArrByteOff = NULL;
    int *newArrPos = NULL;
    int numRequirements = wanted->len;

    for (int i = 0; i < lenAux; i++) {
        // variable created to count the requirements a register fuffils
        int requirementsFufilled = 0;
        
        // fseeking to the register found in index file
        fseek(input, resArr->arrByteOff[i], SEEK_SET);
        // reading the register
        Data *reg = readBinaryRegister(input);

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

        // if all requirements are met, adds the register to the new 
        // arrbyteoffset and arrpos
        if (requirementsFufilled == numRequirements) {
            newLenArr++;
            newArrByteOff = (long long int *)realloc(newArrByteOff, sizeof(long long int) * newLenArr);
            newArrPos = (int *)realloc(newArrPos, sizeof(int) * newLenArr);
            
            newArrByteOff[newLenArr-1] = resArr->arrByteOff[i];
            newArrPos[newLenArr-1] = resArr->arrPos[i];

        }
    }

    // updating the result struct with the new len, arr byteoffset and arr pos
    resArr->length = newLenArr;
    resArr->arrByteOff = newArrByteOff;
    resArr->arrPos = newArrPos;

    return resArr;
}

int isMemberInIndex(Search *wanted, int iteration, char *memberNameIndex) {
    return (strcmp(wanted[iteration].memberName, memberNameIndex) == 0);
}


void printIndex(IndexData *arr, IndexHeader *h) {
    if (arr == NULL) {
        printf("arr nulo\n");
        return;
    }

    for (int i = 0; i < h->numberOfRegisters; i++) {
        printf("Pos: %d Valor: %s || byteoff: %lld\n", i, arr[i].searchKeyStr, arr[i].byteOffset);
    }

}

/*
* Function that searches the register wanted, we search in the index array if the key
* we want its in the search array, otherwise we search sequentially
*/
Result *superSearch(FILE *input, char *memberName, IndexData *arrIndex, IndexHeader *header, Search *s, Header *headerBin) {
    Result *indexSearchResult = NULL;

    //check if will be search by index or linear search
    int verifyWhichSearch = 0;
    while (verifyWhichSearch < s->len) {
        if (isMemberInIndex(s, verifyWhichSearch, memberName))
            break;

        verifyWhichSearch++;
    }

    // no searching by index because none of the members we want to search is indexed
    // in the index file
    if (verifyWhichSearch == s->len) { 
        // using sequential search
        indexSearchResult = sequentialSearch(input, s, headerBin);

        // finding positions at index array by checking offsets found
        indexSearchResult->arrPos = createArrPos(indexSearchResult, arrIndex, header);
    }
    else {
        // binary searching in index array
        indexSearchResult = searchInIndexArr(arrIndex, header, s, verifyWhichSearch, memberName);

        // checking registers found by index due to other search requirements
        indexSearchResult = verifyingRegRequirements(input, indexSearchResult, s);
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

/*
* Funtions that removes a key in the index array 
*/
IndexData *remIndexDataArr(IndexData *indexDataArr, int len, int pos) {
    // throwing the key in 'pos' to the end of the array
    for (int i = pos; i < len; i++) {
        swap(indexDataArr, i, i+1);
    }

    // reallocing and removeing the key that was in pos
    indexDataArr = (IndexData *)realloc(indexDataArr, sizeof(IndexData) * len);
    return indexDataArr;
}


/*
* Function that deletes registers with offset and index position defined by 'toDelete'
* It deletes the registers in the .bin registers input file (by marking them with removed = 1)
* and deletes them from the index data array by removing them and shifting the registers after them
* to a position before
*/
IndexData *superDelete(FILE *input, Result *toDelete, IndexData *indexDataArr, IndexHeader *indexHd, Header *h) {
    // no registers found
    if(toDelete == NULL || toDelete->arrByteOff == NULL || toDelete->length <= 0) {
        return indexDataArr;
    }

    // number of registers to be deleted
    int lenToDelete = toDelete->length;
    // number of registers in index file
    int num = indexHd->numberOfRegisters;
    // registers not found at index file (will not be deleted from index file)
    int notFound = 0;

    char removed = '1';
    // running toDelete vector and deleting data
    for(int i=0; i<lenToDelete; i++) {
        // deleting in input (data) file
        // position of the register to be deleted
        long long int byteOff = toDelete->arrByteOff[i];

        if(byteOff != -1) { // flag
            // fseek to register position in input (data) file
            fseek(input, byteOff, SEEK_SET);

            // overwrites data->removed to '1'
            fwrite(&removed, sizeof(char), 1, input);

            // number of removed in header += 1
            add1FileRemReg(h);
        }

        // deleting in index array
        int position = toDelete->arrPos[i]; // position to be deleted
        // removing and shifting updating indexData Array
        if(toDelete->arrPos[i] != -1) { // if it exists in index array
            indexDataArr = remIndexDataArr(indexDataArr, num - i - 1 + notFound, position);
            
            for(int j=i+1; j<lenToDelete; j++) {
                // shifting back positions bigger than the one deleted
                if (toDelete->arrPos[j] > position)
                    toDelete->arrPos[j] -= 1;
            }
        } 
        else { // didnt find the register in the index file
            notFound++;
        }
        
    }

    // updates the number of registers in index header
    indexHd->numberOfRegisters = num - lenToDelete + notFound;

    return indexDataArr;
}

void printSearchArray(Search *s) {
    printf("SEARCH: \n");
    for(int i=0; i<s->len; i++) {
        if(isIntegerMember(s[i].memberName)) {
            printf("%s %d ", s[i].memberName, s[i].intMember);
        }
        else {
            printf("%s %s ", s[i].memberName, s[i].strMember); 
        }
    }
    printf("\n");
} 

/*
* Function that updates registers with offset and index position defined by 'foundRegisters'
* It updates the registers in the .bin registers input file (by overwriting them)
* and updates them in the index data array by removing them and adding again (if the index member was changed)
* The register's new members are stored in the Search *update
*/
IndexData *superUpdate(FILE *input, Search *update, Result *foundRegisters, IndexData *indexDataArr, IndexHeader *indexHd, Header *h, char *memberName) {
    // no registers found
    if(foundRegisters == NULL || foundRegisters->arrByteOff == NULL || foundRegisters->length <= 0) {
        return indexDataArr;
    }

    // number of positions to be updated
    int lenfoundRegisters = foundRegisters->length;

    // running positions
    for(int i=0; i<lenfoundRegisters; i++) {
        // getting byteoffset of the register to be updated
        long long int byteOff = foundRegisters->arrByteOff[i];
        // getting index data arr position of the register to be updated
        int indexPos = foundRegisters->arrPos[i];

        // fseeking to that position and reading register data
        fseek(input, byteOff, SEEK_SET);
        Data *reg = readBinaryRegister(input);
        int oldSize = registerSize(reg);
        
        // doing updates and checking variable members sizes
        int teste = 0;
        for (int j = 0; j < update->len; j++) {
            if (isIntegerMember(update[j].memberName)) {
                teste += sizeTest(reg, update[j].memberName, NULL, update[j].intMember);
                updateReg(reg, update[j].memberName, NULL, update[j].intMember);
            }
            else {
                teste += sizeTest(reg, update[j].memberName, update[j].strMember, -1);
                updateReg(reg, update[j].memberName, update[j].strMember, -1);
            }
        }

        int flagIndex = 0;
        if (teste >= 0) { // there is space to writing
            writeUpdatedRegister(input, reg, byteOff, oldSize); // overwrites input

            // need to update in index data arr?
            for (int i = 0; i < update->len; i++) {
                // will need to update if updated the indexed member
                if (isMemberInIndex(update, i, memberName))
                    flagIndex = 1;
            }
        }
        else { // no space, needs to write register at end of file

            // calls function to delete the register at byteoff and with indexPos
            // deletes the register both in the input data file and index data arr
            Result *toDelete = createResult(byteOff, indexPos);
            indexDataArr = superDelete(input, toDelete, indexDataArr, indexHd, h);

            // insert the register at the end of index array
            if (isIntegerMember(memberName)) {
                indexDataArr = appendIndexArray(indexDataArr, indexHd, memberName, selectIntegerMember(memberName, reg), NULL, getNexByteOffset(h));
            }
            else {
                indexDataArr = appendIndexArray(indexDataArr, indexHd, memberName, -1, selectStrMember(memberName, reg), getNexByteOffset(h));
            }

            // write the register in the end of input file and updates header
            fseek(input, getNexByteOffset(h), SEEK_SET);
            int newByteOff = writeRegister(input, reg) + bytesFixedMember;
            addByteOffset(h, newByteOff);
            add1FileReg(h);
        }

        if (flagIndex) { // updates in index array (deletes and inserts)
            // calls function to delete the register with indexPos
            // deletes only in the index array
            Result *toDelete = createResult(-1, indexPos);
            indexDataArr = superDelete(input, toDelete, indexDataArr, indexHd, h);

            // insert the register at the end of index array
            if (isIntegerMember(memberName)) {
                indexDataArr = appendIndexArray(indexDataArr, indexHd, memberName, selectIntegerMember(memberName, reg), NULL, byteOff);
            }
            else {
                indexDataArr = appendIndexArray(indexDataArr, indexHd, memberName, -1, selectStrMember(memberName, reg), byteOff);
            }
        }

        for(int j = i+1; j < lenfoundRegisters; j++) {
            // shifting back positions bigger than the one deleted
            if ((flagIndex || teste < 0) && foundRegisters->arrPos[j] > indexPos)
                foundRegisters->arrPos[j] -= 1;
        }
    }
    
    // sorting Index Array again
    if (isIntegerMember(memberName)) {
        sortIndexArrInt(indexDataArr, indexHd->numberOfRegisters);
    }
    else {
        sortIndexArrString(indexDataArr, indexHd->numberOfRegisters);
    }

    return indexDataArr;
}

int getSearchLen(Search *s) {
    return s->len;
}