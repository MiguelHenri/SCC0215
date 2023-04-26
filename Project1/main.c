#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "functions.h"
#include "funcoesFornecidas.c"

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

    return 0;
}