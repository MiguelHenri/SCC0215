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

struct redistribution {
    Key *keys;
    int *pointers;
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
    node->keys[pos1].byteOffSet = node->keys[pos2].byteOffSet;
    
    node->keys[pos2].value = tmpInt;
    node->keys[pos2].byteOffSet = tmpLLint;

    // set pointers
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
    if (index >= TREE_ORDER-1) return;
    n->numKeys++;
    n->keys[index].byteOffSet = offset;
    n->keys[index].value = value;

    sortNode(n);
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
        return mid+1;
    }
    else {
        *RRN = n->pointers[mid];
        return mid;
    }
}

Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader, int **arrayRRN) {
    Result *r = createResult();
    int currentLevel = 0;

    // positioning the file pointer to the next node that has to be read
    fseek(treeFile, HEADERSIZE * (tHeader->root+1), SEEK_SET);
    
    // while not leaf node
    int index = 0;
    int count = 0;
    int RRN = tHeader->root; // starting from root node
    while (currentLevel < tHeader->totalLevels) {

        // saving RRN for insert operation
        // for searching the array is not needed
        if (arrayRRN != NULL) (*arrayRRN)[count] = RRN;

        // reading node
        Node *n = readTreeNode(treeFile);
        //printNode(n);
        int found = 0;

        // running node keys in binary search
        index = binarySearchInNode(n, key, &found, &RRN);

        // check if equal wanted
        if (found) {
            appendResult(r, n->keys[index].byteOffSet, index);
            break;
        }

        currentLevel++;
        //printf("valor do rrn: %d\n", RRN);
        // leaf node, stop searching
        if (RRN == -1) {
            setIndex(r, index);
            break;
        }
        fseek(treeFile, HEADERSIZE * (RRN+1), SEEK_SET);

        // updating counter
        count += 1;
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

void removeKeyFromNode(Node *n, int index) {
    n->keys[index].value = -1;
    n->keys[index].byteOffSet = -1;

    n->pointers[index+1] = -1;
    if (index == 0) 
        n->pointers[0] = -1;

    n->numKeys--;
}

Redistribution *createRedistribution(int len) {
    Redistribution *red = (Redistribution *)malloc(sizeof(Redistribution));
    red->keys = (Key *)malloc(sizeof(Key) * len);
    red->pointers = (int *)malloc(sizeof(int) * (len+1));

    for (int i = 0; i < len; i++) {
        red->keys[i].value = -1;
        red->keys[i].byteOffSet = -1;
        red->pointers[i] = -1;
    }
    red->pointers[len] = -1;

    return red;
}

void sortRedistribuition(Redistribution *r, int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len-i-1; j++) {
        
        }
    
    }
}

void redistribution(Node *left, Node *right, Redistribution *r) {
    int i = 0;
    while (i < left->numKeys) {
        r->keys[i] = left->keys[i];
        r->pointers[i] = left->pointers[i];
        i++;
    }
    r->pointers[i] = left->pointers[i]

    while (i < (right->numKeys +left->numKeys)) {
        r->keys[i] = right->keys[i-left->numKeys];
        r->pointers[i+1] = right->pointers[i-left->numKeys];
        i++;
    }
    r->pointers[i+1] = right->pointers[i-left->numKeys];

}

void split1to2(Node **arrayNode, int pageSon, int pageFather, int pageRoot, Node *root, int key, Result *res) {
    Node *son = createNode();
    Node *father = createNode();

    int indexKey = getIndex(res);
    long long int byteOffKey = getByteoffset(res, 0);

    Redistribution *red = createRedistribution(root->numKeys + 1);

    // the key is going to be promoted
    if (indexKey == TREE_ORDER/2) {
        setKey(father, key, byteOffKey);
        father->pointers[0] = pageRoot;
        father->pointers[1] = pageSon;
        

    }
    // the key is going to be on the new son
    else if (indexKey > TREE_ORDER/2) {
        setKey(son, key, byteOffKey);
    }
    // the key will be on root node
    else {
        

        for (int i = 2; i < TREE_ORDER-1; i++)
            removeKeyFromNode(root, i);
    
        setKey(root, key, byteOffKey);
    }
    
    
}

// void split2to3()

void insertTree(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader) {
    int numLevels = tHeader->totalLevels; // manter atualizado
    int numNodes = tHeader->nextRRN;
    
    int *arrayRRN = (int *)malloc(sizeof(int) * numLevels);
    for (int i=0; i<numLevels; i++) arrayRRN[i] = -1;

    Node **arrayNode = (Node **)malloc(sizeof(Node *) * numNodes);
    for (int i = 0; i < numNodes; i++) arrayNode[i] = NULL;

    // getting RRNs of the nodes to 'update' pointers 
    Result *r = ultraTreeSearch(dataFile, treeFile, key, tHeader, &arrayRRN);

    //for (int i=0; i<numLevels; i++) 
    //    printf("RRN: %d\n", arrayRRN[i]);

    int promotionFlag = 0;
    for (int i = numLevels-1; i >= 0; i--) {
        int currentNode = arrayRRN[i];
    
        // reading node and updating array
        fseek(treeFile, HEADERSIZE * (currentNode+1), SEEK_SET);
        Node *n = readTreeNode(treeFile);
        arrayNode[currentNode] = n;

        // checking if has space to insert
        if (arrayNode[currentNode]->numKeys < 4) { // has space
            // inserting key
            setKey(arrayNode[currentNode], key, getByteoffset(r, 0));
            promotionFlag = 0;
        }
        else { // no space
            int newPageSon = tHeader->nextRRN++;
            int newPageFather = tHeader->nextRRN++;

            // insert
            // sort
            // update pointers

            //         [ 3 ] rrn 2
            // [ 1 2 ] rrn 0  [ 4 5 ] rrn 1 
        }

        if (promotionFlag == 0) break;
            
    }

    // no a direita tem 1 chave a menos se n ficar igual    
    // split 1 - 2 na raiz
    // split 2 - 3 nos outros 

    // redistribuicao primeiro com a pag da esq dps dir
    // ver as condicionais de ocupacao
    // split com a pag direita dps esquerda

    // fazer vetorzao com ponteiros para todos os nos possiveis

}