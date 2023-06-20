#ifndef TREE_H
#define TREE_H

#define TREE_ORDER 5
#define GARBAGE 59
#define HEADERSIZE 76

#include "search.h"

typedef struct key Key;
typedef struct node Node;
typedef struct treeHeader TreeHeader;
typedef struct insertUtils InsertUtils;
typedef struct promotedKey PromotedKey;

void initKey(Key *k);
void initPointers(int *p);
TreeHeader *readTreeHeader(FILE *treeFile);
Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader, InsertUtils *IUtils, Node **arrNode);
void printNode(Node *n);
void printTreeHeader(TreeHeader *t);
void printArvore(FILE *treeFile);
Node **insertTree(FILE *dataFile, FILE *treeFile, int key, long long int byteOffKey, TreeHeader *tHeader, Node **arrayNode);
void writeTreeHeader(FILE *treeFile, TreeHeader *t);
void overwriteTreeFile(FILE *treeFile, Node **arrayNode, TreeHeader *tHeader);
Node **appendArrayNode(Node **arr, Node *node, int newLen);
int getNextRRN(TreeHeader *t);
TreeHeader *createTreeHeader();
void setRoot(TreeHeader *tHeader, int newRoot);
void addTotalKeys(TreeHeader *tHeader);
void addTotalLevels(TreeHeader *tHeader);
void addNextRRN(TreeHeader *tHeader);
void setKey(Node *n, int value, long long int offset);
Node *createNode();
void setNodeLevel(Node *n, int level);
void printArvore2(Node **arrNode, TreeHeader *tHeader);
void treeHeaderSetStatus(TreeHeader *tHeader, char status);

#endif