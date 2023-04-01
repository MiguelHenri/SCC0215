#include <stdio.h>
#include <stdlib.h>
#include "header.h"

struct header {
    char status;
    llint proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};