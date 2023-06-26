#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "header.h"
#include "utils.h"
#include "registers.h"
#include "tree.h"
#include "search.h"

void searchInTree(FILE *dataFile, FILE *treeFile, char *dataMemberName, int numberSearches);
void insertIntoTree(FILE *dataFile, FILE *treeFile, char *dataMemberName, int numInsertions);
void createTree(FILE *dataFile, FILE *treeFile);

#endif