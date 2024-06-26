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
        fclose(input);
        binarioNaTela(nameIndexFile);
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
    else if (operacao == 5) {
        // as with we are deleting, we need to read (to search) 
        // and write (to delete)
        FILE *input = fopen(fileNameInput, "rb+"); 
        if (input == NULL) {
            FILE_ERROR;
            return 0;
        }

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');
        int numberDeletions;
        scanf(" %d", &numberDeletions);

        deleteRegister(input, memberName, indexType, nameIndexFile, numberDeletions);
        fclose(input);
        binarioNaTela(fileNameInput);
        binarioNaTela(nameIndexFile);
    }
    else if (operacao == 6) {
        // as we are inserting, we need to read the header (at least) 
        // and write the register to be inserted
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
        binarioNaTela(nameIndexFile);
    }
    else if (operacao == 7) {
        // as we are updating registers, we need to read (to search)
        // and to write (to update)
        FILE *input = fopen(fileNameInput, "rb+");
        if (input == NULL) {
            FILE_ERROR;
            return 0;
        }

        getc(stdin);
        char *memberName = readMember(stdin, ' ');
        char *indexType = readMember(stdin, ' ');
        char *nameIndexFile = readMember(stdin, ' ');
        int numberUpdates;
        scanf(" %d", &numberUpdates);

        updateRegister(input, memberName, indexType, nameIndexFile, numberUpdates);
        fclose(input);
        binarioNaTela(fileNameInput);
        binarioNaTela(nameIndexFile);
    }

    return 0;
}