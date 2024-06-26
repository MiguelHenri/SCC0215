#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "registers.h"
#include "functions.h"
#include "utils.h"
#include "funcoesFornecidas.h"
#include "index.h"

int main() {
    int operacao;
    char fileNameInput[50];
    char fileNameOutput[50];
    
    scanf("%d %s", &operacao, fileNameInput);

    if (operacao == 1) {
        scanf("%s", fileNameOutput);

        FILE *input = fopen(fileNameInput, "r");
        if (input == NULL) {
            FILE_ERROR;
            return 0;
        }
        FILE *output = createTable(input, fileNameOutput);

        binarioNaTela(fileNameOutput);
    }
    else if (operacao == 2) {
        FILE *input = fopen(fileNameInput, "rb");
        if (input == NULL) {
            FILE_ERROR;
            return 0;
        }
        selectFrom(input);
        fclose(input);
    }
    else if (operacao == 3) {
        FILE *input = fopen(fileNameInput, "rb");
        if (input == NULL) {
            FILE_ERROR;
            return 0;
        }

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');

        createIndexFile(input, memberName, indexType, nameIndexFile);
    }
    else if (operacao == 4) {
        FILE *input = fopen(fileNameInput, "rb");
        if (input == NULL) {
            FILE_ERROR;
            return 0;
        }

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');
        int numberSearches;
        scanf(" %d", &numberSearches);

        searchInBinaryFile(input, memberName, indexType, nameIndexFile, numberSearches);
        fclose(input);
    }
    else if (operacao == 6) {
        FILE *input = fopen(fileNameInput, "rb+");
        if (input == NULL) {
            FILE_ERROR;
            return 0;
        }

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');
        int numberInsertions;
        scanf(" %d", &numberInsertions);



        insertRegister(input, memberName, indexType, nameIndexFile, numberInsertions);
        fclose(input);
        binarioNaTela(fileNameInput);
    }

    return 0;
}