#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "functions.h"
#include "funcoesFornecidas.c"

/*TODO
    funcao para printar no arquivo, cabecalho e data
    funcao de mensagem de erro
    funcao SELECT
    
*/

int main(){
    char fileName[50];
    // scanf("%s\n", fileName);
    FILE *arq = fopen("dados1.csv", "r");
    FILE *out = createTable(arq);
    fclose(arq);

    return 0;
}