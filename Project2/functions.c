#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

// func 8
// void createTreeFile()

// func 9
void searchInTree(FILE *dataFile, FILE *treeFile, char *dataMemberName, int numberSearches) {
    // reading the data header and verifying if it is consistent
    Header *dataFileHeader = readHeaderBinary(dataFile);

    // reading index tree header data and verifying consistency
    TreeHeader *tHeader = readTreeHeader(treeFile);
    
    // printTreeHeader(tHeader);
    // printArvore(treeFile);

    for (int i = 0; i < numberSearches; i++) {
        Result *r = NULL;

        Search *wanted = createSearchArr();

        // if tree is indexed by idCrime, and we are searching idCrime,
        // search in tree
        int key;
        if (strncmp(dataMemberName, "idCrime", 7) == 0 && searchingCrimeId(wanted, &key)) {
            //printf("cheguei");
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

    // looping insertions
    for (int i=0; i<numInsertions; i++) {
        // reading register
        Data *reg = readRegisterStdin2();

        // inserting into reg file
        // fseek and write

        if (strncmp(memberName, "idCrime", 7) == 0) { 
            int key = getDataCrimeId(reg);

            // will insert into tree index file
            insertTree(dataFile, treeFile, key, tHeader);

        }
    }

}