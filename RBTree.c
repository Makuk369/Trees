#include <stdio.h>
#include <stdlib.h>

#define RED 1
#define BLACK 0

// root pos relative to uncle
typedef enum uncleRelation{
    AWAY_LEFT,
    TOWARDS_LEFT,
    AWAY_RIGHT,
    TOWARDS_RIGHT
} UncleRelation;

typedef struct node {
    int value;
    int color; // 1-red, 0-black
    struct node* parent;
    struct node* right;
    struct node* left;
} Node;

Node* createNode(int value);
Node* insertNode(Node* root, int value);

Node* insertFixup(Node* root);

Node* rotateLeft(Node* root);
Node* rotateRight(Node* root);

void indent(int tabCount);
void printTree(Node* root, int level);

int main()
{
    Node* treeRoot = NULL;
    
    printf("\n ---------- 10 ---------- \n");
    treeRoot = insertNode(treeRoot, 10);
    printTree(treeRoot, 0);

    // printf("\n ---------- 5 ---------- \n");
    // treeRoot = insertNode(treeRoot, 5);
    // printTree(treeRoot, 0);

    printf("\n ---------- 20 ---------- \n");
    treeRoot = insertNode(treeRoot, 20);
    printTree(treeRoot, 0);

    printf("\n ---------- 30 ---------- \n");
    treeRoot = insertNode(treeRoot, 30);
    printTree(treeRoot, 0);

    // printf("\n ---------- 7 ---------- \n");
    // treeRoot = insertNode(treeRoot, 7);
    // printTree(treeRoot, 0);

    // printf("\n ---------- -5 ---------- \n");
    // treeRoot = insertNode(treeRoot, -5);
    // printTree(treeRoot, 0);

    // printf("\n ---------- 3 ---------- \n");
    // treeRoot = insertNode(treeRoot, 3);
    // printTree(treeRoot, 0);

    // printf("\n ---------- -10 ---------- \n");
    // treeRoot = insertNode(treeRoot, -10);
    // printTree(treeRoot, 0);

    // printTree(treeRoot, 0);
    // printf("\n");

    // treeRoot->left = rotateLeft(treeRoot->left);
    // treeRoot = rotateRight(treeRoot);
    // treeRoot = deleteNode(treeRoot, 5);

    // printTree(treeRoot, 0);

    // int numToFind = 9;
    // printf("found %d = %s\n", numToFind, findNode(treeRoot, numToFind) ? "true" : "false");

    free(treeRoot);
    return 0;
}

// returns a new node, with black color
Node* createNode(int value){
    Node* result = (Node*)malloc(sizeof(Node));
    if(result != NULL){
        result->parent = NULL;
        result->left = NULL;
        result->right = NULL;
        result->value = value;
        result->color = BLACK;
    }
    return result;
}

Node* insertNode(Node* root, int value){
    static Node* newNode = NULL;

    // add node
    if(root == NULL){
        root = createNode(value);
        newNode = root;
        return root;
    }

    // duplicate value == invalid
    if(value == root->value){
        printf("!!! error: No duplicate values allowed !!!\n");
        return root;
    }

    // go deeper
    if(value < root->value){
        root->left = insertNode(root->left, value);
        root->left->parent = root;
    }
    else{
        root->right = insertNode(root->right, value);
        root->right->parent = root;
    }

    // only call on treeRoot
    if(root->parent == NULL){
        newNode = insertFixup(newNode);

        while (root->parent != NULL)
        {
            root = root->parent;
        }
        return root;
    }

    // return without a change
    // printf("insert root = %d (%p) -left> %p\n", root->value, root, root->left);
    return root;
}

Node* insertFixup(Node* root){
    // CASE 0 = root is treeRoot and its red
    if(root->parent == NULL){
        if(root->color == RED){
            root->color = BLACK;
            return root;
        }
        else{
            return root;
        }
    }
    
    // CASE 1 = root has a black parent (NOT treeRoot)
    if(root->parent->color == BLACK){
        root->color = RED;
        printf("%d - case 1\n", root->value);
        return root;
    }

    // uncle = second child of grandparent
    // has grandparent
    if(root->parent->parent != NULL){
        Node* grandParent = root->parent->parent;
        Node* uncle = NULL;
        UncleRelation uncleRel = AWAY_LEFT;

        // left is parent, then right is uncle
        if((grandParent->left != NULL) && (grandParent->left->value == root->parent->value)){
            uncle = grandParent->right; // can be NULL

            if((root->parent->left != NULL) && (root->value == root->parent->left->value)){
                uncleRel = AWAY_LEFT;
            }
            else{
                uncleRel = TOWARDS_LEFT;
            }
        }
        else{
            uncle = grandParent->left; // can be NULL

            if((root->parent->right != NULL) && (root->value == root->parent->right->value)){
                uncleRel = AWAY_RIGHT;
            }
            else{
                uncleRel = TOWARDS_RIGHT;
            }
        }

        // CASE 2 = root has red parent and red uncle
        if((root->parent->color == RED) && ((uncle != NULL) && (uncle->color == RED))){ // NULL uncle == black uncle
            root->parent->color = BLACK;
            uncle->color = BLACK;
            grandParent->color = RED;
            root->color = RED;
            printf("%d - case 2\n", root->value);
            grandParent = insertFixup(grandParent);
            return root;
        }

        // CASE 3 = root has red parent and black uncle
        if((root->parent->color == RED) && ((uncle == NULL) || (uncle->color == BLACK))){ // NULL uncle == black uncle
            switch (uncleRel){
                case AWAY_LEFT:
                    // printf("fixup = %d -left> %p\n", root->parent->parent->value, root->parent->parent->left);
                    root->parent = rotateRight(root->parent->parent); // grandparent je teraz parent
                    // printf("fixup = %d -left> %p\n", root->parent->right->value, root->parent->right->left);
                    // printf("fixup x(%d) = %p x.parent = %p\n", root->parent->value, root->parent, root->parent->parent);

                    root->color = RED;
                    root->parent->color = BLACK;
                    root->parent->right->color = RED;
                    printf("%d - case 3 (AWAY_LEFT)\n", root->value);
                    return root;
                    break;
                    
                case TOWARDS_LEFT:
                    break;

                case AWAY_RIGHT:
                    root->parent = rotateLeft(root->parent->parent); // grandparent je teraz parent
                    // printf("fixup = %d -left> %p\n", root->parent->right->value, root->parent->right->left);
                    // printf("fixup x(%d) = %p x.parent = %p\n", root->parent->value, root->parent, root->parent->parent);

                    root->color = RED;
                    root->parent->color = BLACK;
                    root->parent->left->color = RED;
                    printf("%d - case 3 (AWAY_RIGHT)\n", root->value);
                    return root;
                    break;

                case TOWARDS_RIGHT:
                    break;
                
                default:
                    break;
            }
        }
    }
    else{ // parent is root
        // CASE 4 = treeRoot is red 
        if(root->parent->color == RED){
            root->parent->color = BLACK;
            printf("%d - case 4\n", root->value);
            return root;
        }
    }

    // CASE 5 = no fix needed
    return root;
}

Node* rotateLeft(Node* root){
    Node* rotNode = root->right;

    root->right = rotNode->left;

    if(rotNode->left != NULL){
        rotNode->left->parent = root;
    }

    rotNode->parent = root->parent; // moze bit aj null

    if(root->parent != NULL){
        if((root->parent->left != NULL) && (root->parent->left->value == root->value)){
            root->parent->left = rotNode;
        }
        else if((root->parent->right != NULL) && (root->parent->right->value == root->value)){
            root->parent->right = rotNode;
        }
    }

    rotNode->left = root;

    root->parent = rotNode;

    return rotNode;
}

Node* rotateRight(Node* root){
    Node* rotNode = root->left;

    root->left = rotNode->right;

    if(rotNode->right != NULL){
        rotNode->right->parent = root;
    }

    rotNode->parent = root->parent; // moze bit aj null

    if(root->parent != NULL){
        if((root->parent->left != NULL) && (root->parent->left->value == root->value)){
            root->parent->left = rotNode;
        }
        else if((root->parent->right != NULL) && (root->parent->right->value == root->value)){
            root->parent->right = rotNode;
        }
    }

    rotNode->right = root;

    root->parent = rotNode;

    return rotNode;
}

void indent(int tabCount){
    for (size_t i = 0; i < tabCount; i++)
    {
        printf("|  ");
    }
}

void printTree(Node* root, int level){
    if(root == NULL){
        indent(level);
        printf("NULL\n");
        return;
    }

    indent(level);
    printf("value = %d - %s\n", root->value, (root->color == RED) ? "RED" : "BLACK");

    indent(level);
    printf("left\n");
    printTree(root->left, level+1);

    indent(level);
    printf("right\n");
    printTree(root->right, level+1);
}
