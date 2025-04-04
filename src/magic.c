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
    unsigned int low;     // lower boundary of the interval (pos)
    unsigned int high;    // high boundary of the interval (pos + length)
    unsigned int maxEnd;  // max endpoint in the subtree
    unsigned int seqNumber;
    int opType;  // 1 for add, -1 for remove
    Color color; 
    INode *left, *right, *parent;
};

struct magic {
    INode *root;
    size_t size; // store number of nodes (operations)
};

/* Prototypes of static functions */
static INode *createNode(int low, int high);
static void destroyTree(INode *root);
static void updateMaxEnd(INode *node);
static void leftRotate(MAGIC m, INode *x);
static void rightRotate(MAGIC m, INode *x);
static void fixup(MAGIC m, INode *z);
static void rbInsert(MAGIC m, INode *z);
static void traverse(INode *node, int pos, INode **operations, int *count, int maxOperations);
static int collectRelevantOperations(MAGIC m, int pos, INode **operations, int maxOperations);
static int inOutMap(INode **operations, int opCount, int pos);
static int outInMap(INode **operations, int opCount, int pos);


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
    if (low < 0 || high < 0) {
        printf("Out of boundaries\n");
        return NULL;
    }
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
 * @param z The newly inserted node (non-NULL)
 */
static void fixup(MAGIC m, INode *z) {
    if (m == NULL || z == NULL) return;

    while (z != m->root && z->parent != NULL && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            // Case: Parent is a left child of grandparent
            INode *uncle = z->parent->parent->right;

            if (uncle != NULL && uncle->color == RED) {
                // Case 1: Uncle is RED -> Recolor
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Move up to grandparent
            } else {
                // Case 2: Uncle is BLACK (or NULL) -> Rotate
                if (z == z->parent->right) {
                    // Case 2a: z is a right child -> Left-rotate parent
                    z = z->parent;
                    leftRotate(m, z);
                }
                // Case 3: Recolor and right-rotate grandparent
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(m, z->parent->parent);
            }
        } else {
            // Symmetric case: Parent is a right child of grandparent
            INode *uncle = z->parent->parent->left;

            if (uncle != NULL && uncle->color == RED) {
                // Case 1: Uncle is RED -> Recolor
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Move up to grandparent
            } else {
                // Case 2: Uncle is BLACK (or NULL) -> Rotate
                if (z == z->parent->left) {
                    // Case 2a: z is a left child -> Right-rotate parent
                    z = z->parent;
                    rightRotate(m, z);
                }
                // Case 3: Recolor and left-rotate grandparent
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(m, z->parent->parent);
            }
        }
    }
    // Ensure root is always black
    m->root->color = BLACK;
}

/**
 * @brief Insert a node into the interval tree (Red-Black approach)
 *
 * @param m Pointer to the MAGIC instance
 * @param z The new node
 */
static void rbInsert(MAGIC m, INode *z) {
    if (m == NULL || z == NULL) return;

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
    
    // Fix Red-Black properties: Recolor and/or rotate if necessary to maintain balance
    fixup(m, z);
}

// PROBLEM HERE maxEnd !!!! (traverse to left subtree)
/**
 * @brief Recursive function to traverse the tree and collect operations
 * @param node Current node in traversal
 * @param pos Position to check
 * @param operations Array to store relevant operations
 * @param count Pointer to the count of collected operations
 * @param maxOperations Maximum number of operations to collect
 */
static void traverse(INode *node, int pos, INode **operations, int *count, int maxOperations) {
    if (node == NULL || *count >= maxOperations){
        return;
    }
    
    // For an operation to be relevant:
    // - It directly affects the position (pos falls within its range)
    // - It indirectly affects the position (operation occurs before pos)
    if ((node->low <= pos && pos < node->high) || // Position falls within range
        (node->low <= pos)) {                    // Operation occurs before position
        operations[(*count)++] = node;
    }
    
    // Check left subtree 
    // PROBLEME ICI !!!!!!!
    if (node->left != NULL && node->left->maxEnd > node->low)
        traverse(node->left, pos, operations, count, maxOperations);
    
    // Check right subtree 
    if (node->right != NULL && node->low <= pos)
        traverse(node->right, pos, operations, count, maxOperations);
}

/**
 * @brief Collects operations that affect a given position
 * @param m Pointer to the MAGIC instance
 * @param pos Position to check
 * @param operations Array to store relevant operations
 * @param maxOperations Maximum number of operations to collect
 * @return Number of operations collected
 */
static int collectRelevantOperations(MAGIC m, int pos, INode **operations, int maxOperations) {
    int count = 0;
    traverse(m->root, pos, operations, &count, maxOperations);
    return count;
}

/**
 * @brief Compare function for sorting operations by sequence number
 */
static int compareBySequence(const void *a, const void *b) {
    INode *nodeA = *(INode **)a;
    INode *nodeB = *(INode **)b;
    return (nodeA->seqNumber - nodeB->seqNumber);
}

static int inOutMap(INode **operations, int opCount, int pos) {
    int result = pos;
    for (int i = 0; i < opCount; i++) {
        INode *op = operations[i];
        if (op->opType == 1) { // Add operation
            if (op->low <= result) {
                result += (op->high - op->low);
            }
        }
        else { // Remove operation
            if (op->low <= result) {
                if (result < op->high)
                    return -1; // Position falls within removed region
                result -= (op->high - op->low);
            }
        }
    }
    return result;
}

static int outInMap(INode **operations, int opCount, int pos) {
    int result = pos;
    
    // Process operations in reverse order 
    for (int i = opCount - 1; i >= 0; i--) {
        INode *op = operations[i];
        
        if (op->opType == 1) { // Undo an add operation
            // If position is within added range, it doesn't exist in input
            if (op->low <= result && result < op->high)
                return -1;
            
            // If position is after the added range, shift backward
            if (result >= op->high)
                result -= (op->high - op->low);
        }
        else { // Undo a remove operation
            // If position is at or after the start of removed section,
            // shift forward by the length of the removed section
            if (op->low <= result)
                result += (op->high - op->low);
        }
    }
    
    return result;
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
    newNode->seqNumber = m->size++; 

    rbInsert(m, newNode);
}

void MAGICremove(MAGIC m, int pos, int length){
    if (m == NULL || length <= 0)
        return;

    INode *newNode = createNode(pos, pos + length);
    if (newNode == NULL)
        return;

    newNode->opType = -1; // -1 for remove
    newNode->seqNumber = m->size++;

    rbInsert(m, newNode);
}

int MAGICmap(MAGIC m, MAGICDirection direction, int pos) {
    if (m == NULL || pos < 0)
        return -1;
    
    // 1. Allocate space for operations
    INode **operations = malloc(m->size * sizeof(INode *));
    if (operations == NULL){
        printf("MAGICmap: Allocation Error\n");
        return -1;
    }
        
    // 2. Collect relevant operations
    int opCount = collectRelevantOperations(m, pos, operations, m->size);
    
    // 3. Sort operations by sequence number
    qsort(operations, opCount, sizeof(INode *), compareBySequence);


    // // test to print operations
    // for (int i = 0; i < opCount; i++) {
    //     printf("%d %d %d\n", operations[i]->opType, operations[i]->low, operations[i]->high);
    // }
    
    
    int result = -1;
    if (direction == STREAM_IN_OUT)
        result = inOutMap(operations, opCount, pos);
    else if (direction == STREAM_OUT_IN)
        result = outInMap(operations, opCount, pos);
    
    free(operations);
    return result;
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

