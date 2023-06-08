#include "tree.h"
#include "registers.h"

struct key {
    int value;
    long long int byteOffSet;
};

struct node {
    int level;
    int numKeys; 
    Key keys[TREE_ORDER-1];
    int pointers[TREE_ORDER];
};

struct treeHeader {
    char status;
    int root;
    int nextRRN;
    int totalLevels;
    int totalKeys;
    char garbage[GARBAGE];
};

void printNode(Node *n) {
    printf("-------------------------\n");
    printf("nivel do no: %d\n", n->level);
    printf("num de chaves: %d\n", n->numKeys);

    for (int i = 0; i < TREE_ORDER-1; i++) {
        printf("chave %d: %d\n", i, n->keys[i].value);
    }
    for (int i = 0; i < TREE_ORDER; i++) {
        printf("ponteiro %d: %d\n", i, n->pointers[i]);
    }
    printf("-------------------------\n");
}

void printTreeHeader(TreeHeader *t) {
    printf("--------header----------\n");
    printf("status: %c\n", t->status);
    printf("root pointer: %d\n", t->root);
    printf("next rrn %d\n", t->nextRRN);
    printf("qtd de niveis: %d\n", t->totalLevels);
    printf("qtd chaves: %d\n", t->totalKeys);
    printf("-------------------------\n");

}

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

void swapKeys(Node *node, int pos1, int pos2) {
    int tmpInt = node->keys[pos1].value;
    long long int tmpLLint = node->keys[pos1].byteOffSet;

    node->keys[pos1].value = node->keys[pos2].value;
    node->keys[pos1].byteOffSet = node->keys[pos2].value;
    
    node->keys[pos2].value = tmpInt;
    node->keys[pos2].value = tmpInt;
}

void sortNode(Node *node) {
    int len = node->numKeys;

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < i - 1; j++) {
            if (node->keys[j].value > node->keys[j+1].value) {
                swapKeys(node, j, j+1);
            }
        }
    }
}


void setNodeLevel(Node *n, int level) {
    n->level = level;
}

void setKey(Node *n, int value, long long int offset) {
    int index = n->numKeys;
    n->keys[index].byteOffSet = offset;
    n->keys[index].value = value;
}

void insertKey(Node *node, int value, long long byteOffSet) {
    // inserting key
    setKey(node, value, byteOffSet);
    // need to sort and evaluate tree pointers
    // split etc
    node->numKeys++;
    sortNode(node);
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

// returns position of key found in node vector keys if found
// or the int value (RRN) of the node to be found
int binarySearchInNode(Node *n, int key, int *found, int *RRN) {
    // searching
    int left, right, mid;
    left = 0;
    right = n->numKeys - 1;
    while (left <= right) {
        mid = (left + right)/2;
        
        // found
        if (n->keys[mid].value == key) {
            *found = 1;
            return mid;
        }
 
        // search smaller values
        else if (n->keys[mid].value > key) right = mid - 1;

        // search bigger values
        else if (n->keys[mid].value < key) left = mid + 1;
    }

    // did not found
    *found = 0;
    if (key > n->keys[mid].value) {
        *RRN = n->pointers[mid+1];
        return n->pointers[mid+1];
    }
    else {
        *RRN = n->pointers[mid];
        return n->pointers[mid];
    }
}

Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader) {
    Result *r = createResult();
    int currentLevel = 0;

    // positioning the file pointer to the next node that has to be read
    fseek(treeFile, HEADERSIZE * (tHeader->root+1), SEEK_SET);
    
    // while not leaf node
    int index = 0;
    int RRN = tHeader->root; // starting from root node
    while (currentLevel < tHeader->totalLevels) {

        // reading node
        Node *n = readTreeNode(treeFile);
        //printNode(n);
        int found = 0;

        // running node keys in binary search
        index = binarySearchInNode(n, key, &found, &RRN);

        // check if equal wanted
        if (found) {
            appendResult(r, n->keys[index].byteOffSet);
            break;
        }

        currentLevel++;
        //printf("valor do rrn: %d\n", RRN);
        // leaf node, stop searching
        if (RRN == -1) {
            break;
        }
        fseek(treeFile, HEADERSIZE * (RRN+1), SEEK_SET);
    }
    
    return r;
}

void printArvore(FILE *treeFile) {
    TreeHeader *th = readTreeHeader(treeFile);
    printTreeHeader(th);

    for (int i = 0; i < th->nextRRN; i++) {
        Node *n = readTreeNode(treeFile);
        printf("RRN = %d\n", i);
        printNode(n);
    }
}