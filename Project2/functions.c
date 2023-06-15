#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

// func 9
void searchInTree(FILE *dataFile, FILE *treeFile, char *dataMemberName, int numberSearches) {
    // reading the data header and verifying if it is consistent
    Header *dataFileHeader = readHeaderBinary(dataFile);

    // reading index tree header data and verifying consistency
    TreeHeader *tHeader = readTreeHeader(treeFile);
    
    //printTreeHeader(tHeader);
    // printArvore(treeFile);

    for (int i = 0; i < numberSearches; i++) {
        Result *r = NULL;

        Search *wanted = createSearchArr();

        // if tree is indexed by idCrime, and we are searching idCrime,
        // search in tree
        int key;
        if (strncmp(dataMemberName, "idCrime", 7) == 0 && searchingCrimeId(wanted, &key)) {
            r = ultraTreeSearch(dataFile, treeFile, key, tHeader, NULL);
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

// func 10
void insertIntoTree(FILE *dataFile, FILE *treeFile, char *memberName, int numInsertions) {
    // reading the data header and verifying if it is consistent
    Header *dataFileHeader = readHeaderBinary(dataFile);

    // reading index tree header data and verifying consistency
    TreeHeader *tHeader = readTreeHeader(treeFile);

    // array with tree nodes
    Node **arrayNode = (Node **)malloc(sizeof(Node *) * getNextRRN(tHeader));
    for (int i = 0; i < getNextRRN(tHeader); i++) arrayNode[i] = NULL;

    // looping insertions
    for (int i=0; i<numInsertions; i++) {
        // reading register
        Data *reg = readRegisterStdin2();

        // inserting into reg file
        // fseek and write

        if (strncmp(memberName, "idCrime", 7) == 0) { 
            int key = getDataCrimeId(reg);

            // will insert into array with tree nodes
            arrayNode = insertTree(dataFile, treeFile, key, tHeader, arrayNode);
        }
    }

    // writing new index file data
    overwriteTreeFile(treeFile, arrayNode, tHeader);
    // writing new index header file data
    writeTreeHeader(treeFile, tHeader);
}

// func 8
void createTree(FILE *dataFile, FILE *treeFile, char *dataMemberName) {
    // reading the data header and verifying if it is consistent
    Header *dataFileHeader = readHeaderBinary(dataFile);

    // creating index tree header 
    TreeHeader *tHeader = createTreeHeader();

    Node **arrayNode = (Node **)malloc(sizeof(Node *) * getNextRRN(tHeader));
    for (int i = 0; i < getNextRRN(tHeader); i++) arrayNode[i] = NULL;

    // search in data file to put in tree file
    for (int i = 0; i < getNumFileRegisters(dataFileHeader); i++) {
        Data * reg = readBinaryRegister(dataFile);

        // crating the first root node
        if (getNextRRN(tHeader) == 0) {
            Node *node = createNode();
            setKey(node, getDataCrimeId(reg), bytesHeader);
            arrayNode =  appendArrayNode(arrayNode, node, 1);
            addNextRRN(tHeader);
            addTotalKeys(tHeader);
            addTotalLevels(tHeader);
            setRoot(tHeader, 0);
        }
        else {
            insertTree(dataFile, treeFile, getDataCrimeId(reg), tHeader, arrayNode);
        }
    }

    // writing new index file data
    overwriteTreeFile(treeFile, arrayNode, tHeader);
    // writing new index header file data
    writeTreeHeader(treeFile, tHeader);
}

void createMiniBin(FILE *newDataFile, FILE *input, int numReg) {
    Header *h = createHeader();
    updateHeader(newDataFile, h);
    writeHeader(newDataFile, h);

    for (int i = 0; i < numReg; i++) {
        Data *reg = readRegister(input);

        writeRegister(newDataFile, reg);
        add1FileReg(h);
    }
    fseek(newDataFile, 0, SEEK_SET);
    writeHeader(newDataFile, h);
}