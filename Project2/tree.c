#include "tree.h"
#include "registers.h"

// to-do include pointers in son split1to2

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

struct promotedKey {
    int value;
    long long int byteOffSet;
    int pointerRRN;
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

PromotedKey *createPromotedKey(int value, int byteOffSet, int pointer) {
    PromotedKey *k = (PromotedKey *)malloc(sizeof(PromotedKey));
    
    k->value = value;
    k->byteOffSet = byteOffSet;
    k->pointerRRN = pointer;

    return k;
}

InsertUtils *creteInsertUtils(int treeLevel) {
    InsertUtils *i = (InsertUtils *)malloc(sizeof(InsertUtils));

    i->arrayPos = (int *)malloc(sizeof(int) * treeLevel);
    i->arrayRRN = (int *)malloc(sizeof(int) * treeLevel);

    return i; 
}

void writeTreeHeader(FILE *treeFile, TreeHeader *t) {
    fseek(treeFile, 0, SEEK_SET);
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
        int swap = 0;
        for (int j = 0; j < len - i - 1; j++) {
            if (node->keys[j].value > node->keys[j+1].value) {
                swapKeys(node, j, j+1);
                swapPointers(node, j+1, j+2);
                swap = 1;
            }
        }

        if (!swap) break;
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
    int left, right, mid = 0;
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

Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader, InsertUtils *IUtils, Node **arrNode) {
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
        Node *n = NULL;
        if (arrNode != NULL) {   
            if (arrNode[RRN] == NULL) {
                n = readTreeNode(treeFile);
                arrNode[RRN] = n;
            }
            else {
                n = arrNode[RRN];
            }
        }
        else {
            n = readTreeNode(treeFile);
        }
            
        // printNode(n);
        int found = 0;

        // running node keys in binary search
        index = binarySearchInNode(n, key, &found, &RRN);
        // printf("sobrevivi a busca binai\n");

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
    fseek(treeFile, 0, SEEK_SET);
    TreeHeader *th = readTreeHeader(treeFile);
    printTreeHeader(th);

    for (int i = 0; i < th->nextRRN; i++) {
        Node *n = readTreeNode(treeFile);
        printf("RRN = %d\n", i);
        printNode(n);
    }
}

void printArvore2(Node **arrNode, TreeHeader *tHeader) {
    
    printTreeHeader(tHeader);

    for (int i = 0; i < tHeader->nextRRN; i++) {
        printf("--RRN: %d--\n", i);
        printNode(arrNode[i]);
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

void nodeLeftShift(Node *n, int begin, int end) {
    int i;
    for (i = begin; i < end; i++) {

        if (i+1 < TREE_ORDER) {
            n->keys[i].value = n->keys[i+1].value;
            n->keys[i].byteOffSet = n->keys[i+1].byteOffSet;
            n->pointers[i] = n->pointers[i+1];
        }

        // setting the previous position to null values
        n->keys[i+1].value = -1;
        n->keys[i+1].byteOffSet = -1;
        n->pointers[i+1] = -1;
    }
    if (i+1 <= TREE_ORDER) {
        n->pointers[i] = n->pointers[i+1];
        n->pointers[i+1] = -1;
    }
}

void nodeRightShift(Node *n, int begin, int end) {
    int i;
    for (i = end; i >= begin; i--) {
        // shifiting
        if (i+1 < TREE_ORDER - 1) {
            n->keys[i+1].value = n->keys[i].value;
            n->keys[i+1].byteOffSet = n->keys[i].byteOffSet;
        }
        
        if (i+2 < TREE_ORDER) {
            n->pointers[i+2] = n->pointers[i+1];
        }

        // setting the previous position to null values
        n->keys[i].value = -1;
        n->keys[i].byteOffSet = -1;
        n->pointers[i+1] = -1;
    }
    n->pointers[i+2] = n->pointers[i+1];
    n->pointers[i+1] = -1;
}

void setPointer(Node *n, int pointerValue, int pointerPos) {
    n->pointers[pointerPos] = pointerValue;
}

Node **appendArrayNode(Node **arr, Node *node, int newLen) {
    arr = (Node**)realloc(arr, HEADERSIZE * newLen);
    arr[newLen-1] = node;

    return arr;
}

Node **split1to2(Node **arrayNode, TreeHeader *tHeader, int pageRoot, Node *root, int indexKey, PromotedKey *toInsert) {
    int pageSon = tHeader->nextRRN;
    int pageFather = tHeader->nextRRN + 1;
    tHeader->nextRRN += 2;
        
    Node *son = createNode();
    Node *father = createNode();

    son->level = tHeader->totalLevels - 1;
    father->level = tHeader->totalLevels;
    root->level = tHeader->totalLevels - 1;
    tHeader->root = pageFather;
    
    int key = toInsert->value;
    long long int byteOffKey = toInsert->byteOffSet;

    int *redis = (int *)malloc(sizeof(int) * TREE_ORDER);
    for (int i=0; i<TREE_ORDER-1; i++)
        redis[i] = arrayNode[pageRoot]->keys[i].value;
    redis[TREE_ORDER-1] = key;

    bubbleSort(redis, TREE_ORDER);

    // the key to be inserted is going to be promoted
    // printf("index key = %d\n", indexKey);
    if (key == redis[TREE_ORDER/2]) {
    
        printf("promovendo: %d no split1-2\n", key);

        // getting mid
        int mid;
        if (TREE_ORDER % 2 == 1)
            mid = TREE_ORDER/2;
        else
            mid = (TREE_ORDER/2)+1;

        // promoting key
        // inserting new key to father
        setKey(father, key, byteOffKey);
        
        // son receiving data from old root
        int newNodePos = 1;
        for (int i = mid; i < TREE_ORDER-1; i++) {
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            setPointer(son, root->pointers[i+1], newNodePos);
            // removing data from old root
            removeKeyFromNode(root, i);
            newNodePos++;
        }
        setPointer(son, toInsert->pointerRRN, 0);
    }
    // the key is going to be on the new son or old root
    else {
        // getting mid (key to be promoted)
        int mid;
        if (key > redis[TREE_ORDER/2]) mid = (TREE_ORDER/2);
        else mid = (TREE_ORDER/2)-1;

        // promoting key
        // father receiving the mid key from the old root
        printf("promovendo: %d no split1-2\n", root->keys[mid].value);
        setKey(father, root->keys[mid].value, root->keys[mid].byteOffSet);
        setPointer(son, root->pointers[mid+1], 0);
        // removing mid key from old root
        removeKeyFromNode(root, mid);
        //printNode(root);

        // son receiving data from old root
        int pointerPos = 1;
        for (int i = (mid+1); i < TREE_ORDER-1; i++) {
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            setPointer(son, root->pointers[i+1], pointerPos);
            // removing data from old root
            removeKeyFromNode(root, i);
            pointerPos++;
        }

        // printf("index key == %d\n", indexKey);
        // deciding which page will receive the key
        if (key > redis[TREE_ORDER/2]) {
            // printf("inserindo chave no son : %d\n", key);
            setKey(son, key, byteOffKey);
            int lixo; int lixo2;
            int pointerPosKey = binarySearchInNode(son, key, &lixo, &lixo2) + 1;
            setPointer(son, toInsert->pointerRRN, pointerPosKey);
        }
        else {
            // printf("inserindo chave no root : %d\n", key);
            setKey(root, key, byteOffKey);
            int lixo; int lixo2;
            int pointerPosKey = binarySearchInNode(root, key, &lixo, &lixo2) + 1;
            setPointer(root, toInsert->pointerRRN, pointerPosKey);
        }
    }

    // setting father pointers
    father->pointers[0] = pageRoot;
    father->pointers[1] = pageSon;

    // appending new nodes to arrayNode
    arrayNode[pageRoot] = root;
    printf("nextRRN: %d\n", tHeader->nextRRN);
    arrayNode = (Node **)realloc(arrayNode, HEADERSIZE * (tHeader->nextRRN));
    printf("sobrevivi\n");
    arrayNode[pageSon] = son;
    arrayNode[pageFather] = father;

    return arrayNode;
}


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

PromotedKey *split2to3(FILE *treeFile, Node **arrNode, int pageFather, int indexFather, PromotedKey *toInsert, TreeHeader *tHeader) {
    // checking which page to split
    int pageLeft = arrNode[pageFather]->pointers[indexFather];
    int pageRight = -1;
    if (indexFather+1 <= TREE_ORDER-1) { // checking if there is space to the right
        pageRight = arrNode[pageFather]->pointers[indexFather+1];
    }
    if (pageRight == -1 || indexFather+1 > TREE_ORDER-1) {
        // printf("split pra esquerda\n");
        // return split
        // call split2-3 changing father to -1
        return split2to3(treeFile, arrNode, pageFather, indexFather-1, toInsert, tHeader);
    }

    // reading pages
    if (arrNode[pageLeft] == NULL) {
         fseek(treeFile, HEADERSIZE * (pageLeft + 1), SEEK_SET);
         Node *n = readTreeNode(treeFile);
         arrNode[pageLeft] = n;
    }
    if (arrNode[pageRight] == NULL) {
        fseek(treeFile, HEADERSIZE *  (pageRight + 1), SEEK_SET);
        Node *n = readTreeNode(treeFile);
        arrNode[pageRight] = n;
    }

    // putting keys into a array
    int lenRedis = TREE_ORDER * 2;
    int *arrRedis = createRedistribArr(arrNode[pageLeft], arrNode[pageRight], 
                    arrNode[pageFather]->keys[indexFather].value, toInsert->value, lenRedis);
    /*if (toInsert->value == 178)
            for (int i=0; i<lenRedis; i++)
                printf("redis[%d]: %d\n", i, arrRedis[i]);*/
    bubbleSort(arrRedis, lenRedis);


    // getting promoted keys positions
    int firstPromo = lenRedis / 3;
    int secondPromo = roundUp( (double)((lenRedis / 3.0) * 2) );

    // getting promoted values
    int firstPromoVal = arrRedis[firstPromo];
    printf("first promo val: %d\n", firstPromoVal);
    int secondPromoVal = arrRedis[secondPromo];

    // creating the new node on te far right
    Node *newNode = createNode();
    int newNodeRRN = tHeader->nextRRN++;
    int newNodePointerFather = indexFather + 2;
    newNode->level = arrNode[pageLeft]->level;
    
    // running right page
    int pos = arrNode[pageRight]->numKeys - 1;
    int auxKeyValue = arrNode[pageRight]->keys[pos].value;
    while (auxKeyValue > secondPromoVal) {
        // moving the right pointer of the key that is greater than second promoted to the new node
        setPointer(newNode, arrNode[pageRight]->pointers[pos+1], newNode->numKeys + 1);

        // inserting the the key that is greater than second promoted into new node
        setKey(newNode, auxKeyValue, arrNode[pageRight]->keys[pos].byteOffSet);

        //removing the key from the old node (right)
        removeKeyFromNode(arrNode[pageRight], pos);

        // updating 'while' variables
        pos -= 1;
        auxKeyValue = arrNode[pageRight]->keys[pos].value;
    }
    
    // removing secondPromoted from old node (right) and saving data
    long long int secPromotedValOffset;
    if (secondPromoVal != toInsert->value) {
        setPointer(newNode, arrNode[pageRight]->pointers[pos+1], 0);
        secPromotedValOffset = arrNode[pageRight]->keys[pos].byteOffSet;
        removeKeyFromNode(arrNode[pageRight], pos);
    }
    else { // secondPromo == toInsert
        if (toInsert->pointerRRN != -1) {
            setPointer(newNode, toInsert->pointerRRN, 0);
            toInsert->pointerRRN = -1;
        }
        secPromotedValOffset = toInsert->byteOffSet;
    }

    // dealing with old father
    // printNode(arrNode[pageRight]);
    nodeRightShift(arrNode[pageRight], 0, arrNode[pageRight]->numKeys - 1);
    // printNode(arrNode[pageRight]);
    setKey(arrNode[pageRight], arrNode[pageFather]->keys[indexFather].value,
            arrNode[pageFather]->keys[indexFather].byteOffSet);
    // removing father
    removeWithoutChangingPointer(arrNode[pageFather], indexFather);
    // printNode(arrNode[pageFather]);

    // running left page
    pos = arrNode[pageLeft]->numKeys - 1;
    auxKeyValue = arrNode[pageLeft]->keys[pos].value;
    while (auxKeyValue > firstPromoVal) {
        // shifting right node
        // printNode(arrNode[pageRight]);
        nodeRightShift(arrNode[pageRight], 0, arrNode[pageRight]->numKeys - 1);
        // printNode(arrNode[pageRight]);

        // setting the pointer of the key greater than 1st promoted val
        setPointer(arrNode[pageRight], arrNode[pageLeft]->pointers[pos+1], 1);
        
        // inserting key from left node to right node
        setKey(arrNode[pageRight], auxKeyValue, arrNode[pageLeft]->keys[pos].byteOffSet);

        // removing key from left node
        removeKeyFromNode(arrNode[pageLeft], pos);

        pos -= 1;
        auxKeyValue = arrNode[pageLeft]->keys[pos].value;
    }

    if (firstPromoVal != toInsert->value) {
        // promoting the 1st new father
        setKey(arrNode[pageFather], firstPromoVal, arrNode[pageLeft]->keys[pos].byteOffSet);

        // setting the pointer from the new father to the right node
        setPointer(arrNode[pageRight], arrNode[pageLeft]->pointers[pos+1], 0);
        removeKeyFromNode(arrNode[pageLeft], pos);

        // inserting the key we wanted
        if (firstPromoVal > toInsert->value) {
            setKey(arrNode[pageLeft], toInsert->value, toInsert->byteOffSet);
            if (toInsert->pointerRRN != -1) {
                int garbage, garbage2;
                int pos = binarySearchInNode(arrNode[pageLeft], toInsert->value, &garbage, &garbage2);
                setPointer(arrNode[pageLeft], toInsert->pointerRRN, pos + 1);
                toInsert->pointerRRN = -1;
            }
        }
        // case: doing the split 2 to 3 with the the page left 
        else if (toInsert->value != secondPromoVal && 
                toInsert->value > arrNode[pageRight]->keys[0].value) { 
            
            // inserting the key in the new node created(far right)
            if (toInsert->value > arrNode[pageRight]->keys[arrNode[pageRight]->numKeys - 1].value) {
                setKey(newNode, toInsert->value, toInsert->byteOffSet);
                if (toInsert->pointerRRN != -1) {
                    int garbage, garbage2;
                    int pos = binarySearchInNode(newNode, toInsert->value, &garbage, &garbage2);
                    setPointer(newNode, toInsert->pointerRRN, pos + 1);
                    toInsert->pointerRRN = -1;
                }
            }
            else { // toInsert not 1st in right page
                setKey(arrNode[pageRight], toInsert->value, toInsert->byteOffSet);
                if (toInsert->pointerRRN != -1) {
                    int garbage, garbage2;
                    int pos = binarySearchInNode(arrNode[pageRight], toInsert->value, &garbage, &garbage2);
                    setPointer(arrNode[pageRight], toInsert->pointerRRN, pos + 1);
                    toInsert->pointerRRN = -1;
                }
            }
        }
        else if (toInsert->value == secondPromoVal) {}
        else { // toInsert will be 1st of the right page
            // nodeRightShift(arrNode[pageRight], 0, arrNode[pageRight]->numKeys - 1);
            setKey(arrNode[pageRight], toInsert->value, toInsert->byteOffSet);
            if (toInsert->pointerRRN != -1) {
                int garbage, garbage2;
                int pos = binarySearchInNode(arrNode[pageRight], toInsert->value, &garbage, &garbage2);
                setPointer(arrNode[pageRight], toInsert->pointerRRN, pos + 1);
                toInsert->pointerRRN = -1;
            }
        }
    }
    else { // firstPromoVal == toInsert
        // inserting toInsert into father
        setKey(arrNode[pageFather], toInsert->value, toInsert->byteOffSet);
    }

    // appending new node
    arrNode[newNodeRRN] = newNode;

    // updating toInsert
    toInsert->value = secondPromoVal;
    toInsert->byteOffSet = secPromotedValOffset;
    toInsert->pointerRRN = newNodeRRN;

    return toInsert;
}

PromotedKey *redistribution(FILE *treeFile, Node **arrayNode, int indexInFather, int pageFather, PromotedKey *toInsert, int *flagSucess) {
    // getting RRNs
    int son = arrayNode[pageFather]->pointers[indexInFather];
    int left = -1; 
    int right = -1;
    Node *n = NULL;

    // reading the left page
    if (indexInFather >= 1) {
        // reading left node
        left = arrayNode[pageFather]->pointers[indexInFather-1];
        if (arrayNode[right] == NULL) {
            fseek(treeFile, HEADERSIZE * (left + 1), SEEK_SET);
            n = readTreeNode(treeFile);
            arrayNode[left] = n;
        }
    }
    // reading the right page
    if (indexInFather <= TREE_ORDER-2) {
        right = arrayNode[pageFather]->pointers[indexInFather+1];
        if (arrayNode[right] == NULL) {
            fseek(treeFile, HEADERSIZE * (right + 1), SEEK_SET);
            n = readTreeNode(treeFile);
            arrayNode[right] = n;
        }
    }

    // there is no space to make redistribution
    if ( (right == -1 || arrayNode[right]->numKeys >= TREE_ORDER-1) && 
         (left == -1 || arrayNode[left]->numKeys >= TREE_ORDER-1) ) {
        *flagSucess = 0;
        return toInsert;
    }

    // we can make a redistribution, with either left or right node
    *flagSucess = 1;

    // check space in left node
    if (left != -1 && arrayNode[left]->numKeys < TREE_ORDER-1) { // has space in left
        int lenRedis = TREE_ORDER + arrayNode[left]->numKeys + 1;

        // creating a large array with key values to find the new promoted father key
        int *redis = createRedistribArr(arrayNode[left], arrayNode[son], 
                    arrayNode[pageFather]->keys[indexInFather-1].value, toInsert->value, lenRedis);


        // sorting vector
        bubbleSort(redis, lenRedis);

        /*if (toInsert->value == 564)
            for (int i=0; i<lenRedis; i++)
                printf("redis[%d]: %d\n", i, redis[i]);*/

        int midPos = lenRedis/2;
        int val_promoted = redis[midPos];
        
        // putting father key down to left
        int fatherValue = arrayNode[pageFather]->keys[indexInFather-1].value;
        long long int fatherOffset = arrayNode[pageFather]->keys[indexInFather-1].byteOffSet;
        setKey(arrayNode[left], fatherValue, fatherOffset);

        // printNode(arrayNode[left]);

        // removing father key, does not change pointers
        removeWithoutChangingPointer(arrayNode[pageFather], indexInFather - 1);
        
        int pos = 0;
        int auxKey = arrayNode[son]->keys[pos].value;

        // moving to the left node the keys on the son node that will have lower value
        // than the new father
        while (arrayNode[left]->numKeys < TREE_ORDER-1 && auxKey < val_promoted) {
            // printf("entrei\n");

            // inserting on the left node
            setPointer(arrayNode[left], arrayNode[son]->pointers[pos], arrayNode[left]->numKeys);
            setKey(arrayNode[left], auxKey, arrayNode[son]->keys[pos].byteOffSet);

            // soft deleting the key that went to the left node, we will not erase the right pointer 
            removeWithoutChangingPointer(arrayNode[son], pos);
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);
        }

        // inserting the first pointer from the son node to the left node, where we moved, or not, 
        // the last key on the son node and deleting it after
        setPointer(arrayNode[left], arrayNode[son]->pointers[0], arrayNode[left]->numKeys);

        // printNode(arrayNode[left]);

        setPointer(arrayNode[son], -1, 0);
    
        if (val_promoted == toInsert->value) {
            return toInsert;
        }
        else if (val_promoted > toInsert->value) {
            // the key we want to insert must be on the left node
            // setting the key on the left node
            setKey(arrayNode[left], toInsert->value, toInsert->byteOffSet);
            if (toInsert->pointerRRN != -1) {
                int garbage; int garbage2;
                int pos = binarySearchInNode(arrayNode[left], toInsert->value, &garbage, &garbage2);
                setPointer(arrayNode[left], toInsert->pointerRRN, pos + 1);
            }

            // updating the to insert value, we will have to insert the first key of the right node
            // on the father node
            toInsert->value = arrayNode[son]->keys[0].value;
            toInsert->byteOffSet = arrayNode[son]->keys->byteOffSet;
            toInsert->pointerRRN = -1;

            // shifitting the son node to left, since we will promote the first key
            removeWithoutChangingPointer(arrayNode[son], 0);
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);
        }
        else { // val_promoted < toInsert->value
            // the key we want to insert must be on the right(son) node

            // saving values to promote later
            int tmpValue = arrayNode[son]->keys[0].value;
            long long int tmpByteOff = arrayNode[son]->keys[0].byteOffSet;

            // shifting son node
            removeWithoutChangingPointer(arrayNode[son], 0);
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);

            // setting the insert value in the son node
            // if (toInsert->value == 1337) printNode(arrayNode[son]);
            setKey(arrayNode[son], toInsert->value, toInsert->byteOffSet);
            //if (toInsert->value == 1337) printNode(arrayNode[son]);
            if (toInsert->pointerRRN != -1) {
                int garbage; int garbage2;
                int pos = binarySearchInNode(arrayNode[son], toInsert->value, &garbage, &garbage2);
                setPointer(arrayNode[son], toInsert->pointerRRN, pos + 1);
            }

            // updating the new to insert value, the promoved key
            toInsert->value = tmpValue;
            toInsert->byteOffSet = tmpByteOff;
            toInsert->pointerRRN = -1;
        }

        // printNode(arrayNode[son]);

        return toInsert;
    }
    else { // has space in right

        int lenRedis = TREE_ORDER + arrayNode[right]->numKeys + 1;

        int *redis = createRedistribArr(arrayNode[son], arrayNode[right], 
                    arrayNode[pageFather]->keys[indexInFather].value, toInsert->value, lenRedis);

        // sorting vector
        bubbleSort(redis, lenRedis);

        int midPos = lenRedis/2;
        int val_promoted = redis[midPos];

        // saving father's values
        int fatherValue = arrayNode[pageFather]->keys[indexInFather].value;
        long long int fatherOffset = arrayNode[pageFather]->keys[indexInFather].byteOffSet;

        // putting father key down to right
        nodeRightShift(arrayNode[right], 0, arrayNode[right]->numKeys);
        setKey(arrayNode[right], fatherValue, fatherOffset);

        // removing father key, does not change pointers
        removeWithoutChangingPointer(arrayNode[pageFather], indexInFather);

        if (val_promoted == toInsert->value) {
            return toInsert;
        }

        // getting promoted data
        int positionPromoted = midPos - 1;
        long long int byteOffPromoted = arrayNode[son]->keys[positionPromoted].byteOffSet;

        // changing pointers
        setPointer(arrayNode[right], arrayNode[son]->pointers[positionPromoted+1], 0);

        // removing 'promoted' from son node
        removeWithoutChangingPointer(arrayNode[son], positionPromoted);

        // inserting key in son
        setKey(arrayNode[son], toInsert->value, toInsert->byteOffSet);
        if (toInsert->pointerRRN != -1) {
            int garbage; int garbage2;
            int pos = binarySearchInNode(arrayNode[son], toInsert->value, &garbage, &garbage2);
            setPointer(arrayNode[son], toInsert->pointerRRN, pos + 1);
        }

        // returning new promoted key
        toInsert->value = val_promoted;
        toInsert->byteOffSet = byteOffPromoted;
        toInsert->pointerRRN = -1;

        return toInsert;
    }
}

Node **insertTree(FILE *dataFile, FILE *treeFile, int key, long long int byteOffkey, TreeHeader *tHeader, Node **arrayNode) {
    int numLevels = tHeader->totalLevels; // manter atualizado
    int numNodes = tHeader->nextRRN;

    InsertUtils *IUtils = creteInsertUtils(tHeader->totalLevels);
    
    // getting RRNs of the nodes to 'update' pointers 
    Result *r = ultraTreeSearch(dataFile, treeFile, key, tHeader, IUtils, arrayNode);

    PromotedKey *toInsert = createPromotedKey(key, byteOffkey, -1);

    int promotionFlag = 0;
    for (int i = numLevels-1; i >= 0; i--) {

        printf("-------i: %d\n", i);
        
        int currentNode = IUtils->arrayRRN[i];
        
        int redistributionSuccess = 0; // flag
    
        printf("inserindo a chave: %d no nó: %d\n", toInsert->value, IUtils->arrayRRN[i]);
        // reading node and updating array
        Node *n;
        if (arrayNode[currentNode] == NULL) {
            fseek(treeFile, HEADERSIZE * (currentNode+1), SEEK_SET);
            n = readTreeNode(treeFile);
            arrayNode[currentNode] = n;
        } 
        else {
            n = arrayNode[currentNode];
        }
        tHeader->totalKeys += 1;
        printf("estou no nivel %d\n", arrayNode[currentNode]->level);

        // if (toInsert->value == 715) {
        //     printArvore2(arrayNode, tHeader);
        //     exit(0);
        // }
        
        // checking if has space to insert
        if (arrayNode[currentNode]->numKeys < 4) { // has space
            // inserting key
            setKey(arrayNode[currentNode], toInsert->value, toInsert->byteOffSet);
            printf("tem espaco\n");
            if (toInsert->pointerRRN != -1) {
                int RRNLixo = 0; int flagLixo = 0; 
                // setting the non NULL pointer into the node, 
                // will only be useful in the split 2-3 scenario
                int pos = binarySearchInNode(arrayNode[currentNode], toInsert->value, &flagLixo, &RRNLixo)+1;
                setPointer(arrayNode[currentNode], toInsert->pointerRRN, pos);
                printf("inserindo RRN: %d, na posicao: %d\n", toInsert->pointerRRN, pos);
                toInsert->pointerRRN = -1;
                // printNode(arrayNode[currentNode]);
            }
            promotionFlag = 0;

        }
        else if (i == 0) { // root node split 1->2
            printf("split 1 to 2\n");
            tHeader->totalLevels += 1;

            arrayNode = split1to2(arrayNode, tHeader, currentNode, n, IUtils->arrayPos[i], toInsert);
        }
        else { // no space
            printf("redistribuicao\n");
            promotionFlag = 1; // will keep looping in higher levels
            toInsert = redistribution(treeFile, arrayNode, IUtils->arrayPos[i-1], IUtils->arrayRRN[i-1], toInsert, &redistributionSuccess);

            if (!redistributionSuccess) {
                printf("split 2 to 3\n");

                arrayNode = (Node **)realloc(arrayNode, HEADERSIZE * (tHeader->nextRRN + 1));
                toInsert = split2to3(treeFile, arrayNode, IUtils->arrayRRN[i-1], IUtils->arrayPos[i-1], toInsert, tHeader);
            }

        }

        if (toInsert->value == -1) 
            printArvore2(arrayNode, tHeader);


        // no key to be promoted
        if (promotionFlag == 0) break;
    }

    return arrayNode;
}

void writeNode(FILE *treeFile, Node *node) {
    int aux;
    long long int llAux;

    aux = node->level;
    fwrite(&aux, sizeof(int), 1, treeFile);

    aux = node->numKeys;
    fwrite(&aux, sizeof(int), 1, treeFile);

    for (int i = 0; i < TREE_ORDER; i++) {
        aux = node->pointers[i];
        fwrite(&aux, sizeof(int), 1, treeFile);

        if (i < TREE_ORDER-1) {
            aux = node->keys[i].value;
            fwrite(&aux, sizeof(int), 1, treeFile);

            llAux = node->keys[i].byteOffSet;
            fwrite(&llAux, sizeof(long long int), 1, treeFile);
        }
    }
}

void overwriteTreeFile(FILE *treeFile, Node **arrayNode, TreeHeader *tHeader) {
    for (int i=0; i < tHeader->nextRRN; i++) {
        if (arrayNode[i] != NULL) {
            fseek(treeFile, HEADERSIZE * (i + 1), SEEK_SET);
            // printf("escrevendo o node :\n");
            // printNode(arrayNode[i]);
            Node *n = arrayNode[i];
            writeNode(treeFile, n);
        }
    }
}

int getNextRRN(TreeHeader *t) {
    return t->nextRRN;
}

void treeHeaderSetStatus(TreeHeader *tHeader, char status) {
    tHeader->status = status;
}