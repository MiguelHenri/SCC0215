#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

/*
* Functionality 8: receives a binary 'dataFile' with registers and creates a 
* binary index file 'treeFile' indexed by 'idCrime' using a B* tree
*/
void createTree(FILE *dataFile, FILE *treeFile) {
    // reading the data header and verifying if it is consistent
    Header *dataFileHeader = readHeaderBinary(dataFile);

    // creating index tree header 
    TreeHeader *tHeader = createTreeHeader();
    Node **arrayNode;

    // writing new index header file data
    treeHeaderSetStatus(tHeader, '0');
    writeTreeHeader(treeFile, tHeader);

    // search in data file to put in tree file
    long long int byteOff = bytesHeader;
    for (int i = 0; i < getNumFileRegisters(dataFileHeader); i++) {

        Data *reg = readBinaryRegister(dataFile);

        // checking if register was removed
        if (getDataRemoved(reg) == '1') continue;
        if (getDataCrimeId(reg) == -1) continue;

        // crating the first root node
        if (getNextRRN(tHeader) == 0) {

            addNextRRN(tHeader);
            addTotalLevels(tHeader);
            setRoot(tHeader, 0);
            
            Node *node = createNode();
            setNodeLevel(node, 1);
            setKey(node, getDataCrimeId(reg), byteOff);
            
            arrayNode = (Node **)malloc(sizeof(Node *) * 1);
            arrayNode[0] = node;
            
        }
        else {
            arrayNode = insertTree(dataFile, treeFile, getDataCrimeId(reg), byteOff, tHeader, arrayNode);

            // updates tree file
            overwriteTreeFile(treeFile, arrayNode, tHeader);
        }
        
        addTotalKeys(tHeader);
        byteOff += registerSize(reg);
    }

    // marking as consistent
    treeHeaderSetStatus(tHeader, '1');

    // writing new index header data
    writeTreeHeader(treeFile, tHeader);

}

/*
* Functionality 9: receives a binary 'dataFile' with registers and a 
* binary index file 'treeFile' indexed by 'dataMemberName', it gets
* searches data from stdin, searches and prints registers found
*/
void searchInTree(FILE *dataFile, FILE *treeFile, char *dataMemberName, int numberSearches) {
    // reading the data header and verifying if it is consistent
    Header *dataFileHeader = readHeaderBinary(dataFile);

    // reading index tree header data and verifying consistency
    TreeHeader *tHeader = readTreeHeader(treeFile);

    for (int i = 0; i < numberSearches; i++) {
        Result *r = NULL;

        Search *wanted = createSearchArr();

        // if tree is indexed by idCrime, and we are searching idCrime, search in tree:
        int key;
        if (strncmp(dataMemberName, "idCrime", 7) == 0 && searchingCrimeId(wanted, &key)) {
            r = ultraTreeSearch(dataFile, treeFile, key, tHeader, NULL, NULL);
            r = verifyingRegRequirements(dataFile, r, wanted);
        }
        // else, needs to do sequential search
        else {
            r = sequentialSearch(dataFile, wanted, dataFileHeader);
        }

        printf("Resposta para a busca %d\n", i+1);
        printResultData(r, dataFile);
    }

}

/*
* Functionality 10: receives a binary 'dataFile' with registers and a 
* binary index file 'treeFile' indexed by 'dataMemberName', it gets
* to be inserted registers data from stdin and inserts to both files
*/
void insertIntoTree(FILE *dataFile, FILE *treeFile, char *dataMemberName, int numInsertions) {
    // reading the data header and verifying if it is consistent
    Header *dataFileHeader = readHeaderBinary(dataFile);

    // reading index tree header data and verifying consistency
    TreeHeader *tHeader = readTreeHeader(treeFile);

    // array with tree nodes
    Node **arrayNode = (Node **)malloc(sizeof(Node *) * getNextRRN(tHeader));
    for (int i = 0; i < getNextRRN(tHeader); i++) arrayNode[i] = NULL;

    long long int byteOffset = getNextByteOffset(dataFileHeader);

    // looping insertions
    for (int i=0; i<numInsertions; i++) {

        // reading register
        Data *reg = readRegisterStdin2();

        if (strncmp(dataMemberName, "idCrime", 7) == 0) { 

            int key = getDataCrimeId(reg);

            // will insert into array with tree nodes
            arrayNode = insertTree(dataFile, treeFile, key, byteOffset, tHeader, arrayNode);

            // add byteoff to header
            addTotalKeys(tHeader);
            byteOffset += registerSize(reg);

            // inserting into data file
            insertRegisterInBinFile(dataFile, reg, dataFileHeader);
        }
        
    }

    // overwriting data header
    writeHeader(dataFile, dataFileHeader);

    // writing new index file data
    overwriteTreeFile(treeFile, arrayNode, tHeader);

    // writing new index header file data
    writeTreeHeader(treeFile, tHeader);

}