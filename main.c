#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "registers.h"
#include "functions.h"
#include "utils.h"
#include "funcoesFornecidas.h"
#include "tree.h"

int main() {
    int operation;
    char fileNameInput[50];
    
    scanf("%d %s", &operation, fileNameInput);

    FILE *dataFile = fopen(fileNameInput, "rb");
    if (dataFile == NULL) {
        FILE_ERROR;
        return 0;
    }

    if (operation == 9) {
        // searching using tree B* data index

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');
        int numSearches;
        scanf(" %d", &numSearches);

        // only reading to search
        FILE *treeFile = fopen(nameIndexFile, "rb");

        searchInTree(dataFile, treeFile, memberName, numSearches);

        fclose(treeFile);
    }
    else if (operation == 10) {
        // inserting new data into index/data file

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');
        int numInsertions;
        scanf(" %d", &numInsertions);

        // will need to read data to find where to put new register
        // and will need to write new register
        FILE *treeFile = fopen(nameIndexFile, "rb+");

        insertIntoTree(dataFile, treeFile, memberName, numInsertions);

        fclose(treeFile);
    }

    fclose(dataFile);

    return 0;
}