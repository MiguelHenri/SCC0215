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
        if (arrNode != NULL && arrNode[RRN] == NULL) {
            n = readTreeNode(treeFile);
            arrNode[RRN] = n;
        }
        else if (arrNode != NULL) {
            n = arrNode[RRN];
        }
            
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
    for (int i = end; i >= begin; i--) {
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

void split1to2(Node **arrayNode, TreeHeader *tHeader, int pageRoot, Node *root, int indexKey, PromotedKey *toInsert) {
    int pageSon = tHeader->nextRRN;
    int pageFather = tHeader->nextRRN + 1;
    tHeader->nextRRN += 2;
        
    Node *son = createNode();
    Node *father = createNode();

    son->level = 1;
    father->level = 0;
    root->level = 1;
    tHeader->root = pageFather;
    
    int key = toInsert->value;
    long long int byteOffKey = toInsert->byteOffSet;

    // the key to be inserted is going to be promoted
    // printf("index key = %d\n", indexKey);
    if (indexKey == TREE_ORDER/2) {
    
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
        for (int i = mid; i < TREE_ORDER-1; i++) {
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            // removing data from old root
            removeKeyFromNode(root, i);
        }
    }
    // the key is going to be on the new son or old root
    else {
        // getting mid (key to be promoted)
        int mid;
        if (indexKey > TREE_ORDER/2) mid = (TREE_ORDER/2);
        else mid = (TREE_ORDER/2)-1;

        // promoting key
        // father receiving the mid key from the old root
        setKey(father, root->keys[mid].value, root->keys[mid].byteOffSet);
        // removing mid key from old root
        removeKeyFromNode(root, mid);
        //printNode(root);

        // son receiving data from old root
        for (int i = (mid+1); i < TREE_ORDER-1; i++) {
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            // removing data from old root
            removeKeyFromNode(root, i);
        }

        // printf("index key == %d\n", indexKey);
        // deciding which page will receive the key
        if (indexKey > TREE_ORDER/2) {
            // printf("inserindo chave no son : %d\n", key);
            setKey(son, key, byteOffKey);
        }
        else {
            // printf("inserindo chave no root : %d\n", key);
            setKey(root, key, byteOffKey);
        }
        
    }

    // setting father pointers
    father->pointers[0] = pageRoot;
    father->pointers[1] = pageSon;

    // appending new nodes to arrayNode
    arrayNode[pageRoot] = root;
    arrayNode = appendArrayNode(arrayNode, son, pageSon + 1);
    arrayNode = appendArrayNode(arrayNode, father, pageSon + 2);
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
    if (indexFather+1 <= TREE_ORDER-1) // checking if there is space to the right
        pageRight = arrNode[pageFather]->pointers[indexFather+1];
    else {
        // return split
        // call split2-3 changing father to -1
        return split2to3(treeFile, arrNode, pageFather, indexFather-1, toInsert, tHeader);
    }

    // reading pages
    // if (arrNode[pageLeft] == NULL) {
    //     fseek(treeFile, HEADERSIZE * (pageLeft + 1), SEEK_SET);
    //     Node *n = readTreeNode(treeFile);
    //     arrNode[pageLeft] = n;
    // }
    if (arrNode[pageRight] == NULL) {
        fseek(treeFile, HEADERSIZE *  (pageRight + 1), SEEK_SET);
        Node *n = readTreeNode(treeFile);
        arrNode[pageRight] = n;
    }

    // putting keys into a array
    int lenRedis = TREE_ORDER * 2;
    int *arrRedis = createRedistribArr(arrNode[pageLeft], arrNode[pageRight], 
                    arrNode[pageFather]->keys[indexFather].value, toInsert->value, lenRedis);
    bubbleSort(arrRedis, lenRedis);

    // getting promoted keys positions
    int firstPromo = lenRedis / 3;
    int secondPromo = roundUp( (double)((lenRedis / 3.0) * 2) );

    // getting promoted values
    int firstPromoVal = arrRedis[firstPromo];
    int secondPromoVal = arrRedis[secondPromo];
    printf("1st promo val %d || 2nd promo val %d\n", firstPromoVal, secondPromoVal);

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
        
    // updating newNode first pointer
    setPointer(newNode, arrNode[pageRight]->pointers[pos+1], 0);
    
    // removing secondPromoted from old node (right) and saving data
    long long int secPromotedValOffset;
    if (secondPromoVal != toInsert->value) {
        secPromotedValOffset = arrNode[pageRight]->keys[pos].byteOffSet;
        removeKeyFromNode(arrNode[pageRight], pos);
    }
    else {
        secPromotedValOffset = toInsert->byteOffSet;
    }

    // dealing with old father
    nodeRightShift(arrNode[pageRight], 0, arrNode[pageRight]->numKeys - 1);
    setKey(arrNode[pageRight], arrNode[pageFather]->keys[indexFather].value,
            arrNode[pageFather]->keys[indexFather].byteOffSet);
    // removing father
    removeWithoutChangingPointer(arrNode[pageFather], indexFather);

    // running left page
    pos = arrNode[pageLeft]->numKeys - 1;
    auxKeyValue = arrNode[pageLeft]->keys[pos].value;
    while (auxKeyValue > firstPromoVal) {
        // shifting right node
        nodeRightShift(arrNode[pageRight], 0, arrNode[pageRight]->numKeys - 1);

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
        }
        // case: doing the split 2 to 3 with the the page left 
        else if (toInsert->value != secondPromoVal && 
                toInsert->value > arrNode[pageRight]->keys[0].value) { 
            
            // inserting the key in the new node created(far right)
            if (toInsert->value > arrNode[pageRight]->keys[arrNode[pageRight]->numKeys - 1].value) {
                setKey(newNode, toInsert->value, toInsert->byteOffSet);
            }
            else { // toInsert not 1st in right page
                setKey(arrNode[pageRight], toInsert->value, toInsert->byteOffSet);
            }
        }   
        else { // toInsert will be 1st of the right page
            nodeRightShift(arrNode[pageRight], 0, arrNode[pageRight]->numKeys - 1);
            setKey(arrNode[pageRight], toInsert->value, toInsert->byteOffSet);
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


    if (indexInFather - 1 >= 0) {
        // reading left node
        left = arrayNode[pageFather]->pointers[indexInFather-1];
        if (arrayNode[right] == NULL) {
            fseek(treeFile, HEADERSIZE * (left + 1), SEEK_SET);
            n = readTreeNode(treeFile);
            arrayNode[left] = n;
        }
    }
    // else return redistribution(treeFile, arrayNode, indexInFather-1, pageFather, toInsert, flagSucess);
    else if (indexInFather + 1 <= TREE_ORDER-1) {
        right = arrayNode[pageFather]->pointers[indexInFather+1];
        if (arrayNode[right] == NULL) {
            fseek(treeFile, HEADERSIZE * (right + 1), SEEK_SET);
            n = readTreeNode(treeFile);
            arrayNode[right] = n;
        }
    }

    // there is no space to make redistribution
    if ( (right != -1 && arrayNode[right]->numKeys >= TREE_ORDER-1) || 
         (left != -1 && arrayNode[left]->numKeys >= TREE_ORDER-1) ) {
        *flagSucess = 0;
        return toInsert;
    }

    // we can make a redistribution, with either left or right node
    *flagSucess = 1;

    // check space in left node
    if (left != -1 && arrayNode[left]->numKeys < TREE_ORDER-1) { // has space in left
        int lenRedis = TREE_ORDER + arrayNode[left]->numKeys + 1;
        // int redis[lenRedis];
        // int i = 0;
        
        // copying into a big vector
        // int len = arrayNode[left]->numKeys;
        // for (int i = 0; i < len; i++) {
        //     redis[i] = arrayNode[left]->keys[i].value;
        // }
        // for (int i = 0; i < arrayNode[son]->numKeys; i++) {
        //     redis[i+len] = arrayNode[son]->keys[i].value;
        // }
        // len += arrayNode[son]->numKeys;
        // redis[len++] = arrayNode[pageFather]->keys[indexInFather-1].value;
        // redis[len] = toInsert->value;

        // creating a large array with key values to find the new promoted father key
        int *redis = createRedistribArr(arrayNode[left], arrayNode[son], 
                    arrayNode[pageFather]->keys[indexInFather].value, toInsert->value, lenRedis);

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

        
        int pos = 0;
        int auxKey = arrayNode[son]->keys[pos].value;

        // moving the the left node, the keys on the son node that will have lower value
        // than the new father
        while (arrayNode[left]->numKeys < TREE_ORDER-1 && auxKey < val_promoted) {

            // inserting on the left node
            setPointer(arrayNode[left], arrayNode[son]->pointers[pos], arrayNode[left]->numKeys);
            setKey(arrayNode[left], auxKey, arrayNode[son]->keys[pos].byteOffSet);

            // soft deleting the key that went to the left node, we will not erase the right pointer 
            removeWithoutChangingPointer(arrayNode[son], pos);
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);
        }

        // inserting the first pointer on the son node to the left node, where we moved, or not, 
        // the last key on the son node and deleting it after
        setPointer(arrayNode[left], arrayNode[son]->pointers[0], arrayNode[left]->numKeys);
        setPointer(arrayNode[son], -1, 0);
    
        if (val_promoted == toInsert->value) {
            return toInsert;
        }
        else if (val_promoted > toInsert->value) {
            // the key we want to insert must be on the left node
            // setting the key on the left node
            setKey(arrayNode[left], toInsert->value, toInsert->byteOffSet);

            // updating the to insert value, we will have to insert the first key of the right node
            // on the father node
            toInsert->value = arrayNode[son]->keys[0].value;
            toInsert->byteOffSet = arrayNode[son]->keys->byteOffSet;

            // shifitting the son node to left, since we will promote the first key
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);
        }
        else { // val_promoted < toInsert->value
            // the key we want to insert must be on the right(son) node

            // saving values to promote later
            int tmpValue = arrayNode[son]->keys[0].value;
            long long int tmpByteOff = arrayNode[son]->keys[0].byteOffSet;

            // setting the insert value in the son node
            setKey(arrayNode[son], toInsert->value, toInsert->byteOffSet);

            // shiftin the node to the left, we will lose the info on the first position
            // of the node, but we saved previously
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);

            // updating the new to insert value, the promoved key
            toInsert->value = tmpValue;
            toInsert->byteOffSet = tmpByteOff;
        }     

        return toInsert;
        

        // if (val_promoted == toInsert->value) {
        //     return toInsert;
        // }
        
        // // getting promoted data
        // int positionPromoted = midPos - arrayNode[son]->numKeys;
        // long long int byteOffPromoted = arrayNode[son]->keys[positionPromoted].byteOffSet;

        // // removing 'promoted' from son node
        // removeWithoutChangingPointer(arrayNode[son], positionPromoted);

        // // inserting key to son
        // setKey(arrayNode[son], toInsert->value, toInsert->byteOffSet);

        // while (arrayNode[left]->numKeys < midPos) {
        //     // changing pointer between nodes
        //     setPointer(arrayNode[left], arrayNode[son]->pointers[0], arrayNode[left]->numKeys);
        //     // changing key and updating pointer
        //     setKey(arrayNode[left], arrayNode[son]->keys[0].value, arrayNode[son]->keys[0].byteOffSet);
        //     // shifting to left
        //     nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);
        // }

        
        // // returning new promoted key
        // toInsert->value = val_promoted;
        // toInsert->byteOffSet = byteOffPromoted;

        // return toInsert;
        
    }
    else { // has space in right
        // reading right node
        // fseek(treeFile, HEADERSIZE * (right + 1), SEEK_SET);
        // Node *n = readTreeNode(treeFile);
        // arrayNode[right] = n;

        int lenRedis = TREE_ORDER + arrayNode[right]->numKeys + 1;
        // int redis[lenRedis];
        // int i = 0;
        
        // // copying into a big vector
        // int len = arrayNode[right]->numKeys;
        // for (int i = 0; i < len; i++) {
        //     redis[i] = arrayNode[right]->keys[i].value;
        // }
        // for (int i = 0; i < arrayNode[son]->numKeys; i++) {
        //     redis[i+len] = arrayNode[son]->keys[i].value;
        // }
        // len += arrayNode[son]->numKeys;
        // redis[len++] = arrayNode[pageFather]->keys[indexInFather-1].value;
        // redis[len] = toInsert->value;

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

        // returning new promoted key
        toInsert->value = val_promoted;
        toInsert->byteOffSet = byteOffPromoted;

        return toInsert;
    }
}

Node **insertTree(FILE *dataFile, FILE *treeFile, int key, long long int byteOffkey, TreeHeader *tHeader, Node **arrayNode) {
    int numLevels = tHeader->totalLevels; // manter atualizado
    int numNodes = tHeader->nextRRN;

    InsertUtils *IUtils = creteInsertUtils(tHeader->totalLevels);
    
    // getting RRNs of the nodes to 'update' pointers 
    Result *r = ultraTreeSearch(dataFile, treeFile, key, tHeader, IUtils, arrayNode);

    // printNode(arrayNode[0]);

    PromotedKey *toInsert = createPromotedKey(key, byteOffkey, -1);

    // for (int i=0; i<numLevels; i++) 
    //     printf("RRN: %d\n", IUtils->arrayRRN[i]);

    int promotionFlag = 0;
    for (int i = numLevels-1; i >= 0; i--) {
        int currentNode = IUtils->arrayRRN[i];
        int redistributionSuccess = 0; // flag
    
        // reading node and updating array
        Node *n;
        if (arrayNode[currentNode] == NULL) {
            fseek(treeFile, HEADERSIZE * (currentNode+1), SEEK_SET);
            n = readTreeNode(treeFile);
            arrayNode[currentNode] = n;
        } else {
            n = arrayNode[currentNode];
        }
        tHeader->totalKeys += 1;
        
        // checking if has space to insert
        if (arrayNode[currentNode]->numKeys < 4) { // has space
            // inserting key
            setKey(arrayNode[currentNode], toInsert->value, toInsert->byteOffSet);
            if (toInsert->pointerRRN != -1) {
                int RRNLixo = 0; int flagLixo = 0; 
                // setting the non NULL pointer into the node, 
                // will only be useful in the split 2-3 scenario
                setPointer(arrayNode[currentNode], toInsert->pointerRRN, 
                            binarySearchInNode(arrayNode[currentNode], 
                                toInsert->value, &flagLixo, &RRNLixo)+1);
            }
            promotionFlag = 0;
        }
        else if (i == 0) { // root node
            // split 1->2

            split1to2(arrayNode, tHeader, currentNode, n, IUtils->arrayPos[i], toInsert);
            
            tHeader->totalLevels += 1;
        }
        else { // no space
            promotionFlag = 1; // will keep looping in higher levels
            
            toInsert = redistribution(treeFile, arrayNode, IUtils->arrayPos[i-1], IUtils->arrayRRN[i-1], toInsert, &redistributionSuccess);

            if (!redistributionSuccess) {
                arrayNode = (Node **)realloc(arrayNode, sizeof(Node *) * (tHeader->nextRRN + 1));
                toInsert = split2to3(treeFile, arrayNode, IUtils->arrayRRN[i-1], IUtils->arrayPos[i-1], toInsert, tHeader);
            }
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
    for (int i=0; i < tHeader->nextRRN; i++) {
        if (arrayNode[i] != NULL) {
            fseek(treeFile, HEADERSIZE * (i + 1), SEEK_SET);
            printf("escrevendo o node :\n");
            printNode(arrayNode[i]);
            writeNode(treeFile, arrayNode[i]);
        }
    }
}

int getNextRRN(TreeHeader *t) {
    return t->nextRRN;
}