#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "header.h"
#include "utils.h"
#include "registers.h"
#include "tree.h"
#include "search.h"

void searchInTree(FILE *dataFile, FILE *treeFile, char *dataMemberName, char *nameIndexFile, int numberSearches);

#endif