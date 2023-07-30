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

/*
* Function that initializes the node key values and offsets
*/
void initKey(Key *k) {
    for (int i = 0; i < TREE_ORDER-1; i++) {
        k[i].value = -1;
        k[i].byteOffSet = -1;
    }
}

/*
* Function that initializes the node pointers
*/
void initPointers(int *p) {
    for (int i = 0; i < TREE_ORDER; i++) {
        p[i] = -1;
    }
}

/*
* Function that creates the header struct of the tree file
*/
TreeHeader *createTreeHeader() {
    // allocating space and initializing data
    TreeHeader *t = (TreeHeader *)malloc(sizeof(TreeHeader));

    t->status = '1'; // consistent
    t->root = -1; 
    t->nextRRN = 0;
    t->totalLevels = 0;
    t->totalKeys = 0;
    for (int i = 0; i < GARBAGE; i++) {
        t->garbage[i] = '$';
    }

    return t;
}

/*
* Function that creates a node struct (a disk page) that will contain tree data
*/
Node *createNode() {
    // allocating space and initializing data
    Node *n = (Node *)malloc(sizeof(Node));
    n->level = -1;
    n->numKeys = 0;
    initKey(n->keys);
    initPointers(n->pointers);

    return n;
}

/*
* Function that creates a Promoted key struct that will be useful when inserting 
* data in the tree
*/
PromotedKey *createPromotedKey(int value, int byteOffSet, int pointer) {
    PromotedKey *k = (PromotedKey *)malloc(sizeof(PromotedKey));
    
    k->value = value;
    k->byteOffSet = byteOffSet;
    k->pointerRRN = pointer;

    return k;
}

/*
* Function that creates a Insert Utils struct that will be useful when inserting in the
* tree because we will be able to save the path we followed in the tree
*/
InsertUtils *creteInsertUtils(int treeLevel) {
    InsertUtils *i = (InsertUtils *)malloc(sizeof(InsertUtils));

    i->arrayPos = (int *)malloc(sizeof(int) * treeLevel);
    i->arrayRRN = (int *)malloc(sizeof(int) * treeLevel);

    return i; 
}

/*
* Function that writes the tree header
*/
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

/*
* Function that sorts the node, will be useful after inserting a key to keep the node sorted
*/
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

/*
* Function to set the node level
*/
void setNodeLevel(Node *n, int level) {
    n->level = level;
}

/*
* Function that inserts a key in a specified node if theres space avaliable
*/
void setKey(Node *n, int value, long long int offset) {
    if (n->numKeys >= TREE_ORDER-1) return;

    // set key in first empty position
    for (int i = 0; i < TREE_ORDER-1; i++) {
        if (n->keys[i].value == value) return;

        if (n->keys[i].value == -1) {
            n->keys[i].byteOffSet = offset;
            n->keys[i].value = value; 
            break;      
        }
    }
    n->numKeys++;

    // sorting node
    sortNode(n);
}

/*
* Function that reads the tree header
*/
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

/*
* The next 5 functions are used to update the tree header in the create tree function
*/
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

void treeHeaderSetStatus(TreeHeader *tHeader, char status) {
    tHeader->status = status;
}

/*
* Function that reads a tree node
*/
Node *readTreeNode(FILE *treeFile) {
    int intAux;
    long long int llintAux;

    Node *n = createNode();

    // reading level
    fread(&intAux, sizeof(int), 1, treeFile);
    n->level = intAux;

    // reading number of keys
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

    // binary search
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

    // did not found, which means we got to go down the tree
    *found = 0;
    if (key > n->keys[mid].value) { // going to bigger velues
        *RRN = n->pointers[mid+1]; // keeping track of the RRNs visited
        return mid+1;
    }
    else { // going to lower velues
        *RRN = n->pointers[mid]; // keeping track of the RRNs visited
        return mid;
    }
}

Result *ultraTreeSearch(FILE *dataFile, FILE *treeFile, int key, TreeHeader *tHeader, InsertUtils *IUtils, Node **arrNode) {
    Result *r = createResult();
    int currentLevel = 0;

    // positioning the file pointer to the next node that has to be read
    fseek(treeFile, HEADERSIZE * (tHeader->root+1), SEEK_SET);
    
    int index = 0;
    int count = 0;
    int RRN = tHeader->root; // starting from root node
    while (currentLevel < tHeader->totalLevels) { // while not leaf node

        // saving RRN for insert operation
        // just for searching operation, the array is not needed
        if (IUtils != NULL) IUtils->arrayRRN[count] = RRN;

        // reading node
        Node *n = NULL;
        if (arrNode != NULL) { // inserting operation  
            if (arrNode[RRN] == NULL) { // we didint visit this node before
                n = readTreeNode(treeFile);
                arrNode[RRN] = n;
            }
            else { // node already visited, we dont have to read the node again
                n = arrNode[RRN];
            }
        }
        else { // searching operation, we gotta read anyway
            n = readTreeNode(treeFile);
        }

        // running node keys in binary search
        int found = 0;
        index = binarySearchInNode(n, key, &found, &RRN);

        // saving index for insert operation
        if (IUtils != NULL) IUtils->arrayPos[count] = index;

        // check if we found the key
        if (found) {
            appendResult(r, n->keys[index].byteOffSet, index);
            setFoundFlag(r, 1);
            break;
        }

        currentLevel++; // going down 1 level of the tree
    
        // leaf node, stop searching
        if (RRN == -1) {
            setIndex(r, index);
            break;
        }

        // reading the next node we have to visit
        fseek(treeFile, HEADERSIZE * (RRN+1), SEEK_SET);

        // updating counter
        count += 1;
    }

    return r;
}

/*
* Function that removes a key, the value and offset, but not its right pointer from a specific node and index
*/
void removeWithoutChangingPointer(Node *n, int index) {
    n->keys[index].value = -1;
    n->keys[index].byteOffSet = -1;

    n->numKeys--;
}

/*
* Function that removes a key, the value and offset, and its right pointer from a specific node and index
*/
void removeKeyFromNode(Node *n, int index) {
    removeWithoutChangingPointer(n, index);
    n->pointers[index+1] = -1;
}

/*
* Shifiting the keys and pointers to the left
*/
void nodeLeftShift(Node *n, int begin, int end) {
    int i;
    for (i = begin; i < end; i++) {

        if (i+1 < TREE_ORDER) { //shifiting
            n->keys[i].value = n->keys[i+1].value;
            n->keys[i].byteOffSet = n->keys[i+1].byteOffSet;
            n->pointers[i] = n->pointers[i+1];

            // setting the previous position to null values
            n->keys[i+1].value = -1;
            n->keys[i+1].byteOffSet = -1;
            n->pointers[i+1] = -1;
        }
        
    }
    
    // moving the last pointer if exists
    if (i+1 <= TREE_ORDER) {
        n->pointers[i] = n->pointers[i+1];
        n->pointers[i+1] = -1;
    }
}

/*
* Shifiting the keys and pointers to the right
*/
void nodeRightShift(Node *n, int begin, int end) {
    int i;
    for (i = end; i >= begin; i--) {

        if (i+1 < TREE_ORDER - 1) { // shifiting
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

    // moving the last pointer
    n->pointers[i+2] = n->pointers[i+1];
    n->pointers[i+1] = -1;
}

void setPointer(Node *n, int pointerValue, int pointerPos) {
    n->pointers[pointerPos] = pointerValue;
}

/*
* Function that appends a node in our array of nodes
*/
Node **appendArrayNode(Node **arr, Node *node, int newLen) {
    arr = (Node**)realloc(arr, HEADERSIZE * newLen);
    arr[newLen-1] = node;

    return arr;
}

/*
* Function that will make the split 1 to 2 when needed (splitting the root node)
*/
Node **split1to2(Node **arrayNode, TreeHeader *tHeader, int pageRoot, Node *root, int indexKey, PromotedKey *toInsert) {
    // getting new nodes RRNs
    int pageSon = tHeader->nextRRN;
    int pageFather = tHeader->nextRRN + 1;

    // updating the next RRN
    tHeader->nextRRN += 2;
        
    // creating new nodes
    Node *son = createNode(); // will be our new right node
    Node *father = createNode();

    // setting nodes data
    son->level = tHeader->totalLevels - 1;
    father->level = tHeader->totalLevels;
    root->level = tHeader->totalLevels - 1;
    tHeader->root = pageFather;
    
    // inserted key data
    int key = toInsert->value;
    long long int byteOffKey = toInsert->byteOffSet;

    // array of possible keys to be promoted
    int *redis = (int *)malloc(sizeof(int) * TREE_ORDER);
    for (int i = 0; i < TREE_ORDER-1; i++)
        redis[i] = arrayNode[pageRoot]->keys[i].value;
    redis[TREE_ORDER-1] = key;

    // sorting
    bubbleSort(redis, TREE_ORDER);

    // the key to be inserted is going to be promoted
    if (key == redis[TREE_ORDER/2]) {
    
        // getting mid
        int mid;
        if (TREE_ORDER % 2 == 1)
            mid = TREE_ORDER/2;
        else
            mid = (TREE_ORDER/2)+1;

        // inserting new key to father (promoting key)
        setKey(father, key, byteOffKey);
        
        // son receiving data from old root, making the tree balanced
        int newNodePos = 1;
        for (int i = mid; i < TREE_ORDER-1; i++) {

            // setting values and pointers on the new node(son)
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            setPointer(son, root->pointers[i+1], newNodePos);
            
            // removing data from old root
            removeKeyFromNode(root, i);
            newNodePos++;
        }

        // setting the far left pointer
        setPointer(son, toInsert->pointerRRN, 0);
    }
    // the key is going to be on the new son or old root
    else {
        // getting mid key (to be promoted)
        int mid;
        if (key > redis[TREE_ORDER/2]) mid = (TREE_ORDER/2);
        else mid = (TREE_ORDER/2)-1;

        // father receiving the mid key (promoted) from the old root
        setKey(father, root->keys[mid].value, root->keys[mid].byteOffSet);
        setPointer(son, root->pointers[mid+1], 0);
        
        // removing mid key from old root
        removeKeyFromNode(root, mid);

        // son receiving data from old root
        int pointerPos = 1;
        for (int i = (mid+1); i < TREE_ORDER-1; i++) {
            // setting values and pointers on the new node(son)
            setKey(son, root->keys[i].value, root->keys[i].byteOffSet);
            setPointer(son, root->pointers[i+1], pointerPos);

            // removing data from old root
            removeKeyFromNode(root, i);
            pointerPos++;
        }

        // deciding which page will receive the key
        if (key > redis[TREE_ORDER/2]) { // the key we wanted to insert is going to be on right(son) node
            // inserting key into son
            setKey(son, key, byteOffKey);
            int lixo; int lixo2;
            int pointerPosKey = binarySearchInNode(son, key, &lixo, &lixo2) + 1;
            
            // setting its pointer
            setPointer(son, toInsert->pointerRRN, pointerPosKey);
        }
        else { // the key we wanted to insert is going to be on left(root) node
            // inserting key into root
            setKey(root, key, byteOffKey);
            int lixo; int lixo2;
            int pointerPosKey = binarySearchInNode(root, key, &lixo, &lixo2) + 1;

            // setting its pointer
            setPointer(root, toInsert->pointerRRN, pointerPosKey);
        }
    }

    // setting father pointers
    father->pointers[0] = pageRoot;
    father->pointers[1] = pageSon;

    // appending new nodes to arrayNode
    arrayNode = (Node **)realloc(arrayNode, HEADERSIZE * (tHeader->nextRRN));
    arrayNode[pageRoot] = root;
    arrayNode[pageSon] = son;
    arrayNode[pageFather] = father;

    return arrayNode;
}

/*
* Function that creates an array with the values of the left and right nodes, aswell as the key we want
* to insert and the father
*/
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

/*
* Function that will insert a pointer if its not -1
*/
void insertPointerIfValid(PromotedKey *toInsert, Node *n) {
    if (toInsert->pointerRRN != -1) {
        // finding the right spot
        int garbage, garbage2;
        int pos = binarySearchInNode(n, toInsert->value, &garbage, &garbage2);

        // setting the pointer and then making it null
        setPointer(n, toInsert->pointerRRN, pos + 1);
        toInsert->pointerRRN = -1;
    }
}

/*
* Function that will deal with the 2 to 3 split, and will return a new key to be inserted(2nd promoted)
*/
PromotedKey *split2to3(FILE *treeFile, Node **arrNode, int pageFather, int indexFather, PromotedKey *toInsert, TreeHeader *tHeader) {
    // checking which page to split
    int pageLeft = arrNode[pageFather]->pointers[indexFather];
    int pageRight = -1;

    // trying to make the split with the right sister
    if (indexFather+1 <= TREE_ORDER-1) { // checking if there is space to the right
        pageRight = arrNode[pageFather]->pointers[indexFather+1];
    }

    // if theres not space on right or it doesnt exist, we will make the split with the left page
    if (pageRight == -1 || indexFather+1 > TREE_ORDER-1) {
        indexFather--; // changing the value to make up for the changes in the split

        // the right page will become the left page
        pageLeft = arrNode[pageFather]->pointers[indexFather];
        pageRight = arrNode[pageFather]->pointers[indexFather+1];
    }

    // reading pages if needed
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
    bubbleSort(arrRedis, lenRedis);

    // getting promoted keys positions
    int firstPromo = lenRedis / 3;
    int secondPromo = roundUp( (double)((lenRedis / 3.0) * 2) );

    // getting promoted values
    int firstPromoVal = arrRedis[firstPromo];
    int secondPromoVal = arrRedis[secondPromo];

    // creating the new node far right
    Node *newNode = createNode();
    int newNodeRRN = tHeader->nextRRN++;
    newNode->level = arrNode[pageLeft]->level;
    
    // running right page
    int pos = arrNode[pageRight]->numKeys - 1;
    int auxKeyValue = arrNode[pageRight]->keys[pos].value;
    while (auxKeyValue > secondPromoVal) {
        // moving the right pointer of the key that is greater than second promoted to the new node
        setPointer(newNode, arrNode[pageRight]->pointers[pos+1], newNode->numKeys + 1);

        // inserting the key that is greater than second promoted into new node
        setKey(newNode, auxKeyValue, arrNode[pageRight]->keys[pos].byteOffSet);

        //removing the key from the old node (right)
        removeKeyFromNode(arrNode[pageRight], pos);

        // updating 'while' variables
        pos -= 1;
        auxKeyValue = arrNode[pageRight]->keys[pos].value;
    }
    
    // checking secondPromoted
    long long int secPromotedValOffset;
    if (secondPromoVal != toInsert->value) {
        // new node gets last pointer from right
        setPointer(newNode, arrNode[pageRight]->pointers[pos+1], 0);

        // saving second promo offset
        secPromotedValOffset = arrNode[pageRight]->keys[pos].byteOffSet;

        // removing second promo from right node
        removeKeyFromNode(arrNode[pageRight], pos);
    }
    else { // secondPromo == toInsert
        if (toInsert->pointerRRN != -1) {
            // new node gets to insert pointer
            setPointer(newNode, toInsert->pointerRRN, 0);

            // updating the pointer to null to not mess with futures insertions
            // when going up the treee
            toInsert->pointerRRN = -1; 
        }
        // saving second promo offset
        secPromotedValOffset = toInsert->byteOffSet;
    }

    // dealing with old father, inserting it in the right node
    nodeRightShift(arrNode[pageRight], 0, arrNode[pageRight]->numKeys - 1);
    setKey(arrNode[pageRight], arrNode[pageFather]->keys[indexFather].value,
            arrNode[pageFather]->keys[indexFather].byteOffSet);
    // removing the old father
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

        // updating 'while' variables
        pos -= 1;
        auxKeyValue = arrNode[pageLeft]->keys[pos].value;
    }

    if (firstPromoVal != toInsert->value) {
        // promoting the 1st new father
        setKey(arrNode[pageFather], firstPromoVal, arrNode[pageLeft]->keys[pos].byteOffSet);

        // setting the pointer from the new father to the right node
        setPointer(arrNode[pageRight], arrNode[pageLeft]->pointers[pos+1], 0);
        removeKeyFromNode(arrNode[pageLeft], pos);

        // inserting the key 'toInsert'
        // insert into left node
        if (toInsert->value < firstPromoVal) {
            setKey(arrNode[pageLeft], toInsert->value, toInsert->byteOffSet);
            insertPointerIfValid(toInsert, arrNode[pageLeft]);
        }
        // inserting into right node
        else if (toInsert->value > firstPromoVal && toInsert->value < secondPromoVal) {
            setKey(arrNode[pageRight], toInsert->value, toInsert->byteOffSet);
            insertPointerIfValid(toInsert, arrNode[pageRight]);
        }
        // inserting into new node
        else if (toInsert->value > secondPromoVal) {
            setKey(newNode, toInsert->value, toInsert->byteOffSet);
            insertPointerIfValid(toInsert, newNode);
        }
    }
    else { // firstPromoVal == toInsert
        // inserting toInsert into father
        setKey(arrNode[pageFather], toInsert->value, toInsert->byteOffSet);
        if (toInsert->pointerRRN != -1) {
            setPointer(arrNode[pageRight], toInsert->pointerRRN, 0);
            toInsert->pointerRRN = -1;
        }
    }

    // appending new node
    arrNode[newNodeRRN] = newNode;

    // updating toInsert
    toInsert->value = secondPromoVal;
    toInsert->byteOffSet = secPromotedValOffset;
    toInsert->pointerRRN = newNodeRRN;

    return toInsert;
}

/*
* Functiong that will make the redistribution if needed between 2 nodes, it will return a new promoted key
* that will be inserted in the next iteration
*/
PromotedKey *redistribution(FILE *treeFile, Node **arrayNode, int indexInFather, int pageFather, PromotedKey *toInsert, int *flagSucess) {
    int left = -1; 
    int right = -1;
    Node *n = NULL;

    // getting RRNs
    int son = arrayNode[pageFather]->pointers[indexInFather];

    // reading the left page
    if (indexInFather >= 1) {
        left = arrayNode[pageFather]->pointers[indexInFather-1];
        if (arrayNode[left] == NULL) {
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

    // has space in left
    if (left != -1 && arrayNode[left]->numKeys < TREE_ORDER-1) {

        int lenRedis = TREE_ORDER + arrayNode[left]->numKeys + 1;

        // creating a large array with key values to find the new promoted father key
        int *redis = createRedistribArr(arrayNode[left], arrayNode[son], 
                    arrayNode[pageFather]->keys[indexInFather-1].value, toInsert->value, lenRedis);

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

        // moving to the left node the keys on the son node that will have lower value
        // than the new father
        while (arrayNode[left]->numKeys < TREE_ORDER-1 && auxKey < val_promoted) {

            // inserting on the left node
            setPointer(arrayNode[left], arrayNode[son]->pointers[pos], arrayNode[left]->numKeys);
            setKey(arrayNode[left], auxKey, arrayNode[son]->keys[pos].byteOffSet);

            // soft deleting the key that went to the left node, we will not erase the right pointer 
            removeWithoutChangingPointer(arrayNode[son], pos);
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);

            auxKey = arrayNode[son]->keys[pos].value;
        }

        // inserting the first pointer from the son node to the left node, where we moved, or not, 
        // the last key on the son node and deleting it after
        setPointer(arrayNode[left], arrayNode[son]->pointers[0], arrayNode[left]->numKeys);
        setPointer(arrayNode[son], -1, 0);
    
        if (val_promoted == toInsert->value) {
            
            // check if has 'promoted' pointer
            if (toInsert->pointerRRN != -1) {
                // sets 'promoted' pointer
                setPointer(arrayNode[son], toInsert->pointerRRN, 0);
                toInsert->pointerRRN = -1; // null
            }
            
            return toInsert;
        }
        else if (val_promoted > toInsert->value) {
            
            // the key we want to insert must be on the left node
            // setting the key on the left node
            setKey(arrayNode[left], toInsert->value, toInsert->byteOffSet);
            insertPointerIfValid(toInsert, arrayNode[left]);
            
            // updating the to insert value, we will have to insert the first key
            // of the son node on the father node
            toInsert->value = arrayNode[son]->keys[0].value;
            toInsert->byteOffSet = arrayNode[son]->keys[0].byteOffSet;
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
            removeWithoutChangingPointer(arrayNode[son], 0); //certo
            nodeLeftShift(arrayNode[son], 0, arrayNode[son]->numKeys);
    
            // setting the insert value in the son node
            setKey(arrayNode[son], toInsert->value, toInsert->byteOffSet);
            insertPointerIfValid(toInsert, arrayNode[son]);

            // updating the new to insert value, the promoved key
            toInsert->value = tmpValue;
            toInsert->byteOffSet = tmpByteOff;
            toInsert->pointerRRN = -1;
        }
        return toInsert;
        
    }
    else { // has space in right
        
        // creating a large array with key values to find the new promoted father key
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
        nodeRightShift(arrayNode[right], 0, arrayNode[right]->numKeys - 1);
        setKey(arrayNode[right], fatherValue, fatherOffset);

        // removing father key, does not change pointers
        removeWithoutChangingPointer(arrayNode[pageFather], indexInFather);

        if (val_promoted == toInsert->value) {
            // inserted value will be promoted
            if (toInsert->pointerRRN != -1) {
                // setting 'promoted' pointer
                setPointer(arrayNode[right], toInsert->pointerRRN, 0);
                toInsert->pointerRRN = -1;
            }

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
        insertPointerIfValid(toInsert, arrayNode[son]);

        // returning new promoted key
        toInsert->value = val_promoted;
        toInsert->byteOffSet = byteOffPromoted;
        toInsert->pointerRRN = -1;

        return toInsert;
    }
}

/*
* Function that will insert a node in the tree, and will return an array of the nodes that will have non
* NULL values in the indexes(RRN) we had to read or modify
*/
Node **insertTree(FILE *dataFile, FILE *treeFile, int key, long long int byteOffkey, TreeHeader *tHeader, Node **arrayNode) {
    int numLevels = tHeader->totalLevels;
    int numNodes = tHeader->nextRRN;

    InsertUtils *IUtils = creteInsertUtils(tHeader->totalLevels);
    
    // getting RRNs of the nodes to 'update' pointers 
    Result *r = ultraTreeSearch(dataFile, treeFile, key, tHeader, IUtils, arrayNode);
    if (getFoundFlag(r)) return arrayNode;

    // creating the key we want to insert
    PromotedKey *toInsert = createPromotedKey(key, byteOffkey, -1);

    // flag for controlling if insertion ended
    int promotionFlag = 0;
    for (int i = numLevels-1; i >= 0; i--) {
        int currentNode = IUtils->arrayRRN[i];
        int redistributionSuccess = 0; // flag
    
        // reading node and updating array
        Node *n = NULL;
        if (arrayNode[currentNode] == NULL) {
            fseek(treeFile, HEADERSIZE * (currentNode+1), SEEK_SET);
            n = readTreeNode(treeFile);
            arrayNode[currentNode] = n;
        } 
        else {
            n = arrayNode[currentNode];
        }
        
        // checking if has space to insert
        if (arrayNode[currentNode]->numKeys < 4) { // has space
            // inserting key
            setKey(arrayNode[currentNode], toInsert->value, toInsert->byteOffSet);

            // setting 'promoted' pointer
            insertPointerIfValid(toInsert, arrayNode[currentNode]);

            // ends insertion
            promotionFlag = 0;
        }
        else if (i == 0) { 
            // root node split 1 to 2
            tHeader->totalLevels += 1;
            // calls split1to2
            arrayNode = split1to2(arrayNode, tHeader, currentNode, n, IUtils->arrayPos[i], toInsert);
        }
        else { // no space
            promotionFlag = 1; // will keep looping in higher levels
            toInsert = redistribution(treeFile, arrayNode, IUtils->arrayPos[i-1], IUtils->arrayRRN[i-1], toInsert, &redistributionSuccess);

            if (!redistributionSuccess) { // does split 2 to 3
                arrayNode = (Node **)realloc(arrayNode, HEADERSIZE * (tHeader->nextRRN + 1));
                toInsert = split2to3(treeFile, arrayNode, IUtils->arrayRRN[i-1], IUtils->arrayPos[i-1], toInsert, tHeader);
            }
        }

        // no key to be promoted
        if (promotionFlag == 0) break;
    }

    return arrayNode;
}

/* 
* Function that writes a node into treeFile
*/
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

/* 
* Function that overwrites treeFile using arrayNode data
*/
void overwriteTreeFile(FILE *treeFile, Node **arrayNode, TreeHeader *tHeader) {
    for (int i=0; i < tHeader->nextRRN; i++) {
        if (arrayNode[i] != NULL) {
            // fseeking to position and writing node
            fseek(treeFile, HEADERSIZE * (i + 1), SEEK_SET);
            Node *n = arrayNode[i];
            writeNode(treeFile, n);
        }
    }
}

/* 
* Function that returns treeHeader next RRN
*/
int getNextRRN(TreeHeader *t) {
    return t->nextRRN;
}