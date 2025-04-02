#include <stdio.h>
#include <stdlib.h>
#include "magic.h"

/**
 * INFO0027: - Programming Techniques (Algorithmics)
 *  Project 1: Bytestream mapper
 * 
 * \file magic.c
 * \brief Implementation of MAGIC ADT
 * \author Boustani Mehdi -- Albashityalshaier Abdelkader
 * \version 0.1
 * \date 04/04/2025
 *
 * Implements the MAGIC ADT using an Interval Tree based on a Red-Black Tree
 * 
 */

/* Opaque Structure for Interval Node */
typedef struct INode_t INode;

/* Enum for tracking colors */
typedef enum {RED, BLACK} Color;

struct INode_t
{
    int low;     // lower boundary of the interval (pos)
    int high;    // high boundary of the interval (pos + length)
    int maxEnd;  // max endpoint in the subtree
    int opType;  // 1 for add, -1 for remove
    int offset;  // offset of the interval
    Color color; 
    INode *left, *right, *parent;
};

struct magic {
    INode *root;
    size_t size; // store number of nodes (!!!! NOT size of bytestream)
};

/* Prototypes of static functions */
static INode *createNode(int low, int high);
static void destroyTree(INode *root);
static void updateMaxEnd(INode *node);
static void leftRotate(MAGIC m, INode *x);
static void rightRotate(MAGIC m, INode *x);
static void fixup(MAGIC m, INode *z);
static void rbInsert(MAGIC m, INode *z);
static int getStreamSize(MAGIC m);


/* Static Functions */

/**
 * @brief Create a new Interval Tree node with a given range.
 *
 * @param low low value of the interval
 * @param high high value of the interval
 * 
 * @return INode* a pointer to the new created Interval Node
 */
static INode *createNode(int low, int high) {
    INode *n = malloc(sizeof(INode));
    if (n == NULL) {
        printf("createNode: Allocation error\n");
        return NULL;
    }

    n->low  = low;
    n->high = high;
    n->maxEnd = high;
    n->color = RED; // by default
    n->parent = NULL;
    n->left   = NULL;
    n->right  = NULL;
    n->offset = 0;
    n->opType = 0;
    return n;
}

/**
 * @brief Recursively destroy a tree
 *
 * @param root Root node of tree/subtree
 */
static void destroyTree(INode *root) {
    if (root == NULL) {
        return;
    }
    
    // Destroy left and right subtrees
    destroyTree(root->left);
    destroyTree(root->right);
    
    // Free the node
    free(root);
}

/**
 * @brief Updates the maxEnd value of a node
 *
 * @param node The node to update
 */
static void updateMaxEnd(INode *node) {
    if (node == NULL) return;
    
    node->maxEnd = node->high;
    
    if (node->left != NULL && node->left->maxEnd > node->maxEnd) {
        node->maxEnd = node->left->maxEnd;
    }
    
    if (node->right != NULL && node->right->maxEnd > node->maxEnd) {
        node->maxEnd = node->right->maxEnd;
    }
}

// A REVOIR !!!!!!!!!!!!!!

/**
 * @brief Performs a left rotation at a given node
 *
 * @param m Pointer to the MAGIC instance
 * @param x The node to rotate around
 */
static void leftRotate(MAGIC m, INode *x) {
    if (m == NULL || x == NULL || x->right == NULL) {
        return;
    }
    
    INode *y = x->right;  // Set y
    
    // Turn y's left subtree into x's right subtree
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    
    // Link x's parent to y
    y->parent = x->parent;
    if (x->parent == NULL) {
        m->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    
    // x on y's left
    y->left = x;
    x->parent = y;
    
    // Update maxEnd values for both nodes
    updateMaxEnd(x);
    updateMaxEnd(y);
}

/**
 * @brief Performs a right rotation at a given node
 *
 * @param m Pointer to the MAGIC instance
 * @param y The node to rotate around
 */
static void rightRotate(MAGIC m, INode *y) {
    if (m == NULL || y == NULL || y->left == NULL) {
        return;
    }
    
    INode *x = y->left;  // Set x
    
    // Turn x's right subtree into y's left subtree
    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }
    
    // Link y's parent to x
    x->parent = y->parent;
    if (y->parent == NULL) {
        m->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    
    // y on x's right
    x->right = y;
    y->parent = x;
    
    // Update maxEnd values for both nodes
    updateMaxEnd(y);
    updateMaxEnd(x);
}


/**
 * @brief Performs rotations and recoloring to maintain red-black properties after inserting a new node
 *
 * @param m Pointer to the MAGIC instance
 * @param z The newly inserted node
 */
static void fixup(MAGIC m, INode *z) {
    if (z == NULL)
        return;

    while (z != m->root && z->parent != NULL && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) { 
            // parent is a left child of grand-parent 

            INode *y = z->parent->parent->right; // uncle of z

            if (y != NULL && y->color == RED) {
                // Case 1: RB property violated (uncle is red) -> Recolor
                z->parent->color = BLACK;       // parent BLACK
                y->color = BLACK;               // uncle BLACK 
                z->parent->parent->color = RED; 
                z = z->parent->parent;        
            } else { 
                if (z == z->parent->right) {
                    // Case 2: z is a right child and Uncle is BLACK -> rotate left
                    z = z->parent;
                    leftRotate(m, z);
                }
    
                if (z->parent != NULL && z->parent->parent != NULL) {
                    // Case 3: z is a left child and Uncle is BLACK -> Recolor and rotate right
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(m, z->parent->parent);
                }
            } 

        } else {
            // Parent is a right child of grand-parent
            INode *y = z->parent->parent->left; // uncle of z

            if (y != NULL && y->color == RED) {
                // Case 1: RB property violated (Uncle is red) -> Recolor
                z->parent->color = BLACK;       // parent BLACK
                y->color = BLACK;               // uncle BLACK 
                z->parent->parent->color = RED; 
                z = z->parent->parent;        
            } else { 
                if (z == z->parent->left) {
                    // Case 2: z is a left child and Uncle is BLACK -> rotate right
                    z = z->parent;
                    rightRotate(m, z);
                }
    
                if (z->parent != NULL && z->parent->parent != NULL) {
                    // Case 3: z is a left child and Uncle is BLACK -> Recolor and rotate left
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(m, z->parent->parent);
                }
            } 
        }
    }
}

/**
 * @brief Insert a node into the interval tree (Red-Black approach)
 *
 * @param m Pointer to the MAGIC instance
 * @param z The new node
 */
static void rbInsert(MAGIC m, INode *z) {
    INode *y = NULL;
    INode *x = m->root;

    while (x != NULL) { // traverse
        y = x; 

        // Update max end with the new node
        if (z->high > x->maxEnd) {
            x->maxEnd = z->high;
        }   

        // Traverse tree according to BST property 
        // (left: Less than, right: Greater or equal)
        if (z->low < x->low) {
            // new node is less than current node
            x = x->left;
        } else {
            // new node is greater than or equal to current node
            x = x->right;
        }
    }

    // Set y as parent of new node
    z->parent = y;

    // Place x according to BST children property
    // (left: less than, right: Greater or equal)
    if (y == NULL) { // tree is empty
        m->root = z;
    } else if (z->low < y->low) {
        y->left = z;
    } else {
        y->right = z;
    }
    
    // Recolor and/or rotate if necessary to maintain balance
    fixup(m, z);

    // Update maxEnd values for all ancestors of the new node
    while(y != NULL) {
        int maxEnd = y->high;

        if (y->left != NULL && y->left->maxEnd > maxEnd) {
            maxEnd = y->left->maxEnd;
        }

        if (y->right != NULL && y->right->maxEnd > maxEnd) {
            maxEnd = y->right->maxEnd;
        }

        y->maxEnd = maxEnd;
        y = y->parent;
    }
}

static int computeOffset(MAGIC m, int pos) {

    if(m == NULL || m->root == NULL) return 0;

    INode *current = m->root;

    int offset = 0;

    int currIntervalSize = 0;

    while(current != NULL){
        if(pos < current->low){
            current = current->left;
        }
        else{
            currIntervalSize = current->high - current->low;

            if(current->opType == -1){
                offset -= currIntervalSize;
            }
            else if(current->opType == 1){
                offset += currIntervalSize;
            }

            current = current->right;
        }
    }

    return offset;
}


// THIS IS JUST A BRUTE ALGO, TO UPDATE LATER
static void handleOverlap(MAGIC m, INode *newNode){
    assert(m != NULL && m->root != NULL && newNode);

    INode *current = m->root;
    INode *overlapping = NULL;

    while(current != NULL){
        // Check if overlapping is present
        if(newNode->low <= current->high && newNode->high >= current->low){
            overlapping = current;

            // We found an overlapping, we can quit the search
            break;
        }

        //Continue the search
        if(newNode->low < current->low){
            current = current->left;
        }
        else{
            current = current->right;
        }
    }
    // If there is no overlapping, we insert the new node normally
    if(!overlapping){
        rbInsert(m, newNode);
        return;
    }

    // ----------------------- CASES OF OVERLAPPING -----------------------

    // 1) Subsumed (the interval is just included in another, we merge them)
    if(newNode->low >= overlapping->low && newNode->high <= overlapping->high){
        overlapping->offset += newNode->offset;
        free(newNode);
        return;
    }

    // 2) Partial overlap (the interval is partially included in another, we split them in 3)
    if (newNode->low < overlapping->low || newNode->high > overlapping->high) {

        if(newNode->low < overlapping->low){
            INode *leftNode = createNode(newNode->low, overlapping->low - 1);
            leftNode->opType = newNode->opType;
            leftNode->offset = newNode->offset;
            rbInsert(m, leftNode);
        }

        if(newNode->high > overlapping->high){
            INode *rightNode = createNode(overlapping->high + 1, newNode->high);
            rightNode->opType = newNode->opType;
            rightNode->offset = newNode->offset;
            rbInsert(m, rightNode);
        }

        overlapping->offset += newNode->offset;

        free(newNode);
        
        return;
    }

    // 3) Union of 2 intervals
    INode *next = nextInterval(m, overlapping);
    if(next != NULL && newNode->high >= next->low){
        overlapping->high = (newNode->high > next->high) ? newNode->high : next->high;
        overlapping->offset += newNode->offset + next->offset;
        
        rbRemove(m, next);
        free(next);
        free(newNode);

        return;
    }

}

static INode *nextInterval(MAGIC m, INode *node){
    assert(m != NULL && node != NULL);

    // If there is a right child, we store the lowest element of it
    if(node->right != NULL){
        INode *current = node->right;
        while(current->left != NULL){
            current = current->left;
        }
        return current;
    }


    // Else: get the first ancestor of left child
    while(node->parent != NULL){
        if(node == node->parent->left){
            return node->parent;
        }
        node = node->parent;
    }
    
    return NULL;
}
    


/* Implementation of API */

MAGIC MAGICinit() {
    MAGIC m = malloc(sizeof(struct magic));
    if (m == NULL) {
        printf("MAGICinit: Allocation error\n");
        return NULL;
    }

    m->root = NULL;
    m->size = 0;

    return m;
}

void MAGICadd(MAGIC m, int pos, int length){
    if (m == NULL || length <= 0)
        return;

    INode *newNode = createNode(pos, pos + length);
    if (newNode == NULL)
        return;

    newNode->opType = 1; // 1 for add

    newNode->offset = computeOffset(m, pos);
    
    handleOverlap(m, newNode);

    m->size++;
}

void MAGICremove(MAGIC m, int pos, int length){
    if (m == NULL || length <= 0)
        return;

    INode *newNode = createNode(pos, pos + length);
    if (newNode == NULL)
        return;

    newNode->opType = -1; // -1 for remove

    newNode->offset = computeOffset(m, pos);

    handleOverlap(m, newNode);

    m->size++;
}

int MAGICmap(MAGIC m, MAGICDirection direction, int pos){
    assert(m != NULL && (direction == STREAM_IN_OUT || direction == STREAM_OUT_IN));

    INode *current = m->root;

    int offset = 0;

    int currIntervalSize = 0;

    while(current != NULL){
        if(current->low <= pos && current->high > pos){
            // If we want to get an interval removed from IN to OUT, it doesn't exist in OUT stream
            // Or, if we want to get one added from OUT to INT, it doesn't exist in IN stream
            if((direction == STREAM_IN_OUT && current->opType == -1) ||
            (direction == STREAM_OUT_IN && current->opType == 1) ){
                return -1;
            }
            // Else: Continue to search
        }

        if(pos < current->low){
            current = current->left;
        }
        else{
            currIntervalSize = current->high - current->low;

            // If the interval is removed and we go from IN to OUT, we substract the interval size
            // If the interval is removed and we go from OUT to IN, we add the interval size
            if(current->opType == -1){
                offset += (direction == STREAM_IN_OUT ? -1 : 1) * currIntervalSize;
            }
            // If the interval is added and we go from OUT to IN, we add the interval size
            // If the interval is added and we go from IN to OUT, we substract the interval size
            else if(current->opType == 1){
                offset += (direction == STREAM_IN_OUT ? 1 : -1) * currIntervalSize;
            }

            current = current->right;
        }
    }

    return pos + offset;
    
}

void MAGICdestroy(MAGIC m){
    if (m == NULL) {
        return;
    }
    
    // Destroy the tree
    destroyTree(m->root);
    
    // Free MAGIC 
    free(m);
}