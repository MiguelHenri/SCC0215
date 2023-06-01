#ifndef TREE_H
#define TREE_H

#define TREE_ORDER 5
#define GARBAGE 59

#include "search.h"

typedef struct key Key;
typedef struct node Node;
typedef struct treeHeader TreeHeader;

void initKey(Key *k);
void initPointers(int p[]);

#endif