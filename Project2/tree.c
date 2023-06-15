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

struct insertUtils {
    int *arrayRRN;
    int *arrayPos;
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

InsertUtils *creteInsertUtils(int treeLevel) {
    InsertUtils *i = (InsertUtils *)malloc(sizeof(InsertUtils));

    i->arrayPos = (int *)malloc(sizeof(int) * treeLevel);
    i->arrayRRN = (int *)malloc(sizeof(int) * treeLevel);

    return i; 
}

void writeTreeHeader(FILE *treeFile, TreeHeader *t) {
    fwrite(&(t->status), sizeof(char), 1, treeFile);
    fwrite(&(t->root), sizeof(int), 1, treeFile);
    fwrite(&(t->nextRRN), sizeof(int), 1, treeFile);
    fwrite(&(t->totalLevels), sizeof(int), 1, treeFile);
    fwrite(&(t->totalKeys), sizeof(int), 1, treeFile);

    char trash = '$';
    for (int i = 0; i < GARBAGE; i++)
        fwrite(&trash, sizeof(char), 1, treeFile);
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
}

void swapPointers(Node *node, int pos1, int pos2) {
    int tmp = node->pointers[pos1];
    node->pointers[pos1] = node->pointers[pos2];
    node->pointers[pos2] = tmp;
}

void sortNode(Node *node) {
    int len = node->numKeys;

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (node->keys[j].value > node->keys[j+1].value) {
                swapKeys(node, j, j+1);
                swapPointers(node, j+1, j+2);
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

    // set key in first empty position
    for (int i = 0; i < TREE_ORDER-1; i++) {
        if (n->keys[i].value == -1) {
            n->keys[i].byteOffSet = offset;
            n->keys[i].value = value; 
            break;      
        }
    }

    // sorting node
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

void addNextRRN(TreeHeader *tHeader) {
    tHeader->nextRRN++;
}

void addTotalLevels(TreeHeader *tHeader) {
    tHeader->totalLevels++;
}

void addTotalKeys(TreeHeader *tHeader) {
    tHeader->totalKeys++;
}

void setRoot(TreeHeader *tHeader, int newRoot) {
    tHeader->root = newRoot;
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

Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader, InsertUtils *IUtils) {
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
        if (IUtils != NULL) IUtils->arrayRRN[count] = RRN;

        // reading node
        Node *n = readTreeNode(treeFile);
        //printNode(n);
        int found = 0;

        // running node keys in binary search
        index = binarySearchInNode(n, key, &found, &RRN);

        // saving index for insert operation
        if (IUtils != NULL) IUtils->arrayPos[count] = index;

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

    n->numKeys--;
}

void removeWithoutChangingPointer(Node *n, int index) {
    n->keys[index].value = -1;
    n->keys[index].byteOffSet = -1;

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

void nodeLeftShift(Node *n, int begin, int end) {
    for (int i = begin; i < end; i++) {

        n->keys[i].value = n->keys[i+1].value;
        n->keys[i].byteOffSet = n->keys[i+1].byteOffSet;
        n->pointers[i] = n->pointers[i+1];
        if (i+2 <= TREE_ORDER) 
            n->pointers[i+1] = n->pointers[i+2];

        // setting the previous position to null values
        n->keys[i+1].value = -1;
        n->keys[i+1].byteOffSet = -1;
        n->pointers[i+1] = -1;
    }
}

void nodeRightShift(Node *n, int begin, int end) {
    for (int i = begin; i < end; i++) {
        // shifiting
        n->keys[i+1].value = n->keys[i].value;
        n->keys[i+1].byteOffSet = n->keys[i].byteOffSet;
        if (i+2 <= TREE_ORDER)
            n->pointers[i+2] = n->pointers[i+1];
        n->pointers[i+1] = n->pointers[i];
        

        // setting the previous position to null values
        n->keys[i].value = -1;
        n->keys[i].byteOffSet = -1;
        n->pointers[i] = -1;
    }
}

void setPointer(Node *n, int pointerValue, int pointerPos) {
    n->pointers[pointerPos] = pointerValue;
}

Node **appendArrayNode(Node **arr, Node *node, int newLen) {
    arr = (Node**)realloc(arr, sizeof(Node *) * newLen);
    arr[newLen-1] = node;

    return arr;
}

void split1to2(Node **arrayNode, int pageSon, int pageFather, int pageRoot, Node *root, int key, Result *res) {
    Node *son = createNode();
    Node *father = createNode();

    int indexKey = getIndex(res);
    long long int byteOffKey = getByteoffset(res, 0);

    // the key to be inserted is going to be promoted
    if (indexKey == roundUp(((double)TREE_ORDER)/2)) {
        // getting mid
        int mid = TREE_ORDER/2;

        // promoting key
        // father receiving the mid key from the old root
        setKey(father, key, byteOffKey);
        
        // son receiving data from old root
        for (int i = (mid+1); i < TREE_ORDER-1; i++) {
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            // removing data from old root
            removeKeyFromNode(root, i);
        }
    }
    // the key is going to be on the new son or old root
    else {
        // getting mid (key to be promoted)
        int mid;
        if (indexKey > TREE_ORDER/2) mid = (TREE_ORDER/2) + 1;
        else mid = TREE_ORDER/2;

        // promoting key
        // father receiving the mid key from the old root
        setKey(father, root->keys[mid].value, root->keys[mid].byteOffSet);
        // removing mid key from old root
        removeKeyFromNode(root, mid);

        // son receiving data from old root
        for (int i = (mid+1); i < TREE_ORDER-1; i++) {
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            // removing data from old root
            removeKeyFromNode(root, i);
        }

        // deciding which page will receive the key
        if (indexKey > TREE_ORDER/2) setKey(son, key, byteOffKey);
        else setKey(root, key, byteOffKey);
        

    }
    // setting father pointers
    father->pointers[0] = pageRoot;
    father->pointers[1] = pageSon;

    // appending new nodes to arrayNode
    arrayNode = appendArrayNode(arrayNode, son, pageSon);
    arrayNode = appendArrayNode(arrayNode, father, pageSon+1);
}

// void split2to3()

int *createRedistribArr(Node *left, Node *right, int fatherKey, int key, int lenArr) {
    int *arr = (int *)malloc(sizeof(int) * lenArr);
    int count = 0;
    for (int i = 0; i < left->numKeys; i++) {
        arr[i] = left->keys[i].value;
        count += 1;
    }

    for (int i = 0; i < right->numKeys; i++) {
        arr[count+i] = right->keys[i].value;
    }
    count += right->numKeys;
    arr[count++] = fatherKey;
    arr[count] = key;

    return arr;
}

int redistribution(FILE *treeFile, Node **arrayNode, int indexInFather, int pageFather, int byteOffKey, int key) {
    // getting RRNs
    int son = arrayNode[pageFather]->pointers[indexInFather];
    int left; int right;
    if (indexInFather - 1 >= 0) {
        // reading left node
        left = arrayNode[pageFather]->pointers[indexInFather-1];
        fseek(treeFile, HEADERSIZE * (left + 1), SEEK_SET);
        Node *n = readTreeNode(treeFile);
        arrayNode[left] = n;
    }
    if (indexInFather <= TREE_ORDER-1)
        right = arrayNode[pageFather]->pointers[indexInFather+1];

    // check space in left node
    if (arrayNode[left]->numKeys < TREE_ORDER-1) {
        int lenRedis = TREE_ORDER + arrayNode[left]->numKeys + 1;
        int redis[lenRedis];
        int i = 0;
        
        // copying into a big vector
        int len = arrayNode[left]->numKeys;
        for (int i = 0; i < len; i++) {
            redis[i] = arrayNode[left]->keys[i].value;
        }
        for (int i = 0; i < arrayNode[son]->numKeys; i++) {
            redis[i+len] = arrayNode[son]->keys[i].value;
        }
        len += arrayNode[son]->numKeys;
        redis[len++] = arrayNode[pageFather]->keys[indexInFather-1].value;
        redis[len] = key;

        // sorting vector
        bubbleSort(redis, lenRedis);

        int midPos = lenRedis/2;
        int val_promoted = redis[midPos];
        
        // putting father key down to left
        int fatherValue = arrayNode[pageFather]->keys[indexInFather-1].value;
        long long int fatherOffset = arrayNode[pageFather]->keys[indexInFather-1].byteOffSet;
        setKey(arrayNode[left], fatherValue, fatherOffset);
        // removing father key, does not change pointers
        removeWithoutChangingPointer(arrayNode[pageFather], indexInFather - 1);

        if (val_promoted == key) {
            // promoting key to the father
            setKey(arrayNode[pageFather], key, byteOffKey);
        }
        else {
            // promoting promoted to the father
            // getting byteoff
            int positionPromoted = midPos - arrayNode[son]->numKeys;
            long long int byteOffPromoted = arrayNode[son]->keys[positionPromoted].byteOffSet;

            // inserting promoted in father
            setKey(arrayNode[pageFather], val_promoted, byteOffPromoted);

            // removing 'promoted' from son node
            removeWithoutChangingPointer(arrayNode[son], positionPromoted);

            // inserting key to son
            setKey(arrayNode[son], key, byteOffKey);
        }

        while (arrayNode[left]->numKeys < midPos) {
            // changing pointer between nodes
            setPointer(arrayNode[left], arrayNode[son]->pointers[0], arrayNode[left]->numKeys);
            // changing key and updating pointer
            setKey(arrayNode[left], arrayNode[son]->keys[0].value, arrayNode[son]->keys[0].byteOffSet);
            // shifting to left
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);
        }
    }
    else {
        // reading right node
        fseek(treeFile, HEADERSIZE * (right + 1), SEEK_SET);
        Node *n = readTreeNode(treeFile);
        arrayNode[right] = n;

        int lenRedis = TREE_ORDER + arrayNode[right]->numKeys + 1;
        int redis[lenRedis];
        int i = 0;
        
        // copying into a big vector
        int len = arrayNode[right]->numKeys;
        for (int i = 0; i < len; i++) {
            redis[i] = arrayNode[right]->keys[i].value;
        }
        for (int i = 0; i < arrayNode[son]->numKeys; i++) {
            redis[i+len] = arrayNode[son]->keys[i].value;
        }
        len += arrayNode[son]->numKeys;
        redis[len++] = arrayNode[pageFather]->keys[indexInFather-1].value;
        redis[len] = key;

        // sorting vector
        bubbleSort(redis, lenRedis);

        int midPos = lenRedis/2;
        int val_promoted = redis[midPos];

        // putting father key down to right
        int fatherValue = arrayNode[pageFather]->keys[indexInFather-1].value;
        long long int fatherOffset = arrayNode[pageFather]->keys[indexInFather-1].byteOffSet;
        nodeRightShift(arrayNode[right], 0, arrayNode[right]->numKeys);
        setKey(arrayNode[right], fatherValue, fatherOffset);
        // removing father key, does not change pointers
        removeWithoutChangingPointer(arrayNode[pageFather], indexInFather - 1);

        if (val_promoted == key) {
            // promoting key to the father
            setKey(arrayNode[pageFather], key, byteOffKey);
        }
        else {
            // promoting promoted to the father
            // getting byteoff
            int positionPromoted = midPos - 1;
            long long int byteOffPromoted = arrayNode[son]->keys[positionPromoted].byteOffSet;

            // inserting promoted in father
            setKey(arrayNode[pageFather], val_promoted, byteOffPromoted);

            // changing pointers
            setPointer(arrayNode[right], arrayNode[son]->pointers[positionPromoted+1], 0);

            // removing 'promoted' from son node
            removeWithoutChangingPointer(arrayNode[son], positionPromoted);

            // inserting key in son
            setKey(arrayNode[son], key, byteOffKey);
        }

    }
}

Node **insertTree(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader, Node **arrayNode) {
    int numLevels = tHeader->totalLevels; // manter atualizado
    int numNodes = tHeader->nextRRN;
    
    int *arrayRRN = (int *)malloc(sizeof(int) * numLevels);
    for (int i=0; i<numLevels; i++) arrayRRN[i] = -1;

    InsertUtils *IUtils = creteInsertUtils(tHeader->totalLevels);
    
    // getting RRNs of the nodes to 'update' pointers 
    Result *r = ultraTreeSearch(dataFile, treeFile, key, tHeader, IUtils);

    //for (int i=0; i<numLevels; i++) 
    //    printf("RRN: %d\n", arrayRRN[i]);

    int promotionFlag = 0;
    for (int i = numLevels-1; i >= 0; i--) {
        int currentNode = IUtils->arrayRRN[i];
    
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
            int pageSon = tHeader->nextRRN++;
            int pageFather = tHeader->nextRRN++;

            

            split1to2(arrayNode, pageSon, pageFather, currentNode, n, key, r);
            tHeader->totalLevels += 1;
        }

        if (promotionFlag == 0) break;
            
    }

    return arrayNode; 

    // no a direita tem 1 chave a menos se n ficar igual    
    // split 1 - 2 na raiz
    // split 2 - 3 nos outros 

    // redistribuicao primeiro com a pag da esq dps dir
    // ver as condicionais de ocupacao
    // split com a pag direita dps esquerda

    // fazer vetorzao com ponteiros para todos os nos possiveis

}

void writeNode(FILE *treeFile, Node *node) {
    fwrite(&(node->level), sizeof(int), 1, treeFile);
    fwrite(&(node->numKeys), sizeof(int), 1, treeFile);

    for (int i = 0; i < TREE_ORDER; i++) {
        fwrite(&(node->pointers[i]), sizeof(int), 1, treeFile);
        if (i < TREE_ORDER-1) {
            fwrite(&(node->keys[i].value), sizeof(int), 1, treeFile);
            fwrite(&(node->keys[i].byteOffSet), sizeof(long long int), 1, treeFile);
        }
    }

}

void overwriteTreeFile(FILE *treeFile, Node **arrayNode, TreeHeader *tHeader) {
    for (int i=0; i < tHeader->nextRRN - 1; i++) {
        if (arrayNode[i] != NULL) {
            fseek(treeFile, HEADERSIZE * (i + 1), SEEK_SET);
            writeNode(treeFile, arrayNode[i]);
        }
    }
}

int getNextRRN(TreeHeader *t) {
    return t->nextRRN;
}

