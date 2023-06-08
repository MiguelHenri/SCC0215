#ifndef TREE_H
#define TREE_H

#define TREE_ORDER 5
#define GARBAGE 59
#define HEADERSIZE 76

#include "search.h"

typedef struct key Key;
typedef struct node Node;
typedef struct treeHeader TreeHeader;

void initKey(Key *k);
void initPointers(int p[]);
TreeHeader *readTreeHeader(FILE *treeFile);
Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader);
void printNode(Node *n);
void printTreeHeader(TreeHeader *t);
void printArvore(FILE *treeFile);
#endif