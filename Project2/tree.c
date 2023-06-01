#include "tree.h"
#include "registers.h"

struct key {
    int value;
    long long int byteOffSet;
};

struct node {
    int level;
    int numKeys; 
    Key keys[4];
    int pointers[5];
};

struct treeHeader {
    char status;
    int root;
    int nextRRN;
    int totalLevels;
    int totalKeys;
    char garbage[GARBAGE];
};

TreeHeader *createTreeHeader() {

    // allocating space and initializing data
    TreeHeader *t = (TreeHeader *)malloc(sizeof(TreeHeader));
    t->status = '1'; // consistent
    t->root = -1; 
    t->nextRRN = 0;
    t->totalLevels = 0;
    t->totalKeys = 0;
    for (int i=0; i<GARBAGE; i++) {
        t->garbage[i] = '$';
    }

    return t;
}

Node *createNode() {
    
    // allocating space and initializing data
    Node *n = (Node *)malloc(sizeof(Node));
    n->level = -1;
    n->numKeys = 0;
    initKey(n->keys);
    initPointers(n->pointers);

    return n;
}

void initKey(Key *k) {
    for (int i = 0; i < TREE_ORDER-1; i++) {
        k[i].value = -1;
        k[i].byteOffSet = -1;
    }
}

void initPointers(int *p) {
    for (int i = 0; i < TREE_ORDER; i++) {
        p[i] = -1;
    }
}

void insertKey(Node *n, int value, long long byteOffSet) {
    // inserting key
    setKey(n, value, byteOffSet);
    // need to sort and evaluate tree pointers
    // split etc
    n->numKeys++;
}

void setNodeLevel(Node *n, int level) {
    n->level = level;
}

void setKey(Node *n, int value, long long int offset) {
    int index = n->numKeys;
    n->k[index].byteOffSet = offset;
    n->k[index].value = value;
}

TreeHeader *readTreeHeader(FILE *treeFile) {
    int intAux;
    char charAux;
    
    TreeHeader *header = createTreeHeader();

    fread(&charAux, sizeof(char), 1, treeFile);
    header->status = charAux;

    if (header->status == '0') { // not consistent
        FILE_ERROR;
        exit(0);
    }

    fread(&intAux, sizeof(int), 1, treeFile);
    header->root = intAux;

    fread(&intAux, sizeof(int), 1, treeFile);
    header->nextRRN = intAux;

    fread(&intAux, sizeof(int), 1, treeFile);
    header->totalLevels = intAux;

    fread(&intAux, sizeof(int), 1, treeFile);
    header->totalKeys = intAux;

    fseek(treeFile, GARBAGE, SEEK_CUR);

    return header;
}

Node *readTreeNode(FILE *treeFile) {
    int intAux;
    long long int llintAux;

    Node *n = createNode();

    fread(&intAux, sizeof(int), 1, treeFile);
    n->level = intAux;

    fread(&intAux, sizeof(int), 1, treeFile);
    n->numKeys = intAux;

    for (int i = 0; i < TREE_ORDER; i++) {

        // reading the pointers
        fread(&intAux, sizeof(int), 1, treeFile);
        n->pointers[i] = intAux;

        // there are keys to be read
        if (i < TREE_ORDER-1) {
            fread(&intAux, sizeof(int), 1, treeFile);
            n->keys[i].value = intAux;

            fread(&llintAux, sizeof(long long int), 1, treeFile);
            n->keys[i].byteOffSet = llintAux;
        }
    }

    return n;
}

int binarySearchInNode(Key *k) {

}

Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, Search *wanted, TreeHeader *tHeader) {
    Result *r = createResult();
    int currentLevel = 0;

    // need to read the header
    
    // while not leaf node
    while (currentLevel < tHeader->totalLevels) {
        // check treeFile
        // how to do fseek in treeFile?
        // is it sorted?

        // read node
        Node *n = readTreeNode(treeFile);
        
        // running node keys
        // calls binary search

        // check if equal wanted

        // case idCrime
        // case numeroArtigo

        // else try > or < and keep searching on pointers (sons)
    }

}