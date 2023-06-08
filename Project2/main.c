#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "registers.h"
#include "functions.h"
#include "utils.h"
#include "funcoesFornecidas.h"
#include "tree.h"

int main() {
    int operacao;
    char fileNameInput[50];
    
    scanf("%d %s", &operacao, fileNameInput);

    if (operacao == 9) {
        // searching using tree B* data index
        FILE *dataFile = fopen(fileNameInput, "rb");
        if (dataFile == NULL) {
            FILE_ERROR;
            return 0;
        }

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');
        int numSearches;
        scanf(" %d", &numSearches);

        FILE *treeFile = fopen(nameIndexFile, "rb");

        // printf("abri todos os arq\n");

        searchInTree(dataFile, treeFile, memberName, nameIndexFile, numSearches);

        fclose(dataFile);
        fclose(treeFile);
    }

    return 0;
}