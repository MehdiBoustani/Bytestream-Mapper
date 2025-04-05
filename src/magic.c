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
 * \version 2.2
 * \date 04/04/2025
 *
 * Implements the MAGIC ADT using an Interval Tree based on a Red-Black Tree
 * Sorted by sequence number with interval metadata (minSubtree) for pruning 
 * 
 */

/* Opaque Structure for Interval Node */
typedef struct INode_t INode;

/* Enum for tracking colors */
typedef enum {RED=0, BLACK=1} Color;

/* Enum for operation type */
typedef enum {REMOVE=0, ADD=1} OperationType;

struct INode_t {
    unsigned int low;  // lower boundary of the interval (pos)
    unsigned int high;      // high boundary of the interval (pos + length)
    unsigned int seqNumber;   // sequence number to track chronological order
    OperationType opType;  // 1 for add, -1 for remove

    unsigned int minSubtree;  // minimum low value in this subtree (for pruning)
    
    Color color;  // Color of node in RB tree 
    INode *left, *right, *parent;
};

/* MAGIC ADT */
struct magic {
    INode *root;
    size_t size;           // store number of nodes (operations)
};

/* Prototypes of static functions */
static INode *createNode(int low, int high, OperationType OperationType, unsigned int seqNumber);
static void destroyTree(INode *root);
static void updateMinSubtree(INode *node);
static void leftRotate(MAGIC m, INode *x);
static void rightRotate(MAGIC m, INode *x);
static void rbInsertFixup(MAGIC m, INode *newNode);
static void rbInsert(MAGIC m, INode *newNode);
static int mapInOut(INode *node, int pos);
static int mapOutIn(INode *node, int pos);

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

void MAGICadd(MAGIC m, int pos, int length) {
    if (m == NULL || length <= 0 || pos < 0)
        return;
    // create a new operation node (ADD)
    INode *newNode = createNode(pos, pos + length, ADD, m->size);
    if (newNode == NULL)
        return;

    rbInsert(m, newNode);
}

void MAGICremove(MAGIC m, int pos, int length) {
    if (m == NULL || length <= 0 || pos < 0)
        return;

    // create a new operation node (REMOVE)
    INode *newNode = createNode(pos, pos + length, REMOVE, m->size);
    if (newNode == NULL)
        return;

    rbInsert(m, newNode);
}

int MAGICmap(MAGIC m, enum MAGICDirection direction, int pos) {
    if (m == NULL || pos < 0)
        return -1;
    
    if (m->root == NULL) 
        return pos; // No operations, mapping is identity
    
    // Choose mapping function based on direction
    if (direction == STREAM_IN_OUT) {
        return mapInOut(m->root, pos);
    } else { // STREAM_OUT_IN
        return mapOutIn(m->root, pos);
    }
}

void MAGICdestroy(MAGIC m) {
    if (m == NULL) {
        return;
    }
    
    // Destroy the tree
    destroyTree(m->root);
    
    // Free MAGIC structure
    free(m);
}


/* Static Functions Implementation */

/**
 * @brief Create a new Interval Tree node with a given range.
 *
 * @param low low value of the interval
 * @param high high value of the interval
 * @param opType operation type (1 for add, -1 for remove)
 * @param seqNumber sequence number for ordering
 * 
 * @return INode* a pointer to the new created Interval Node
 */
static INode *createNode(int low, int high, OperationType opType, unsigned int seqNumber) {
    if (low < 0 || high < 0 || low > high) {
        printf("createNode: Invalid interval boundaries\n");
        return NULL;
    }
    
    INode *n = malloc(sizeof(INode));
    if (n == NULL) {
        printf("createNode: Allocation error\n");
        return NULL;
    }

    n->low = low;
    n->high = high;
    n->seqNumber = seqNumber;
    n->opType = opType;
    n->color = RED;     // New nodes are RED by default
    n->parent = NULL;
    n->left = NULL;
    n->right = NULL;
    
    // Initialize minSubtree with the node's own low value
    n->minSubtree = low;
    
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
 * @brief Update the minSubtree value for a node based on its low value and children
 *
 * @param node Node to update
 */
static void updateMinSubtree(INode *node) {
    if (node == NULL) return;
    
    // Ensure default value
    node->minSubtree = node->low;
    
    // See left child's minimum low value
    if (node->left != NULL && node->left->minSubtree < node->minSubtree) {
        node->minSubtree = node->left->minSubtree;
    }
    
    // See right child's minimum low value
    if (node->right != NULL && node->right->minSubtree < node->minSubtree) {
        node->minSubtree = node->right->minSubtree;
    }
}

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
    
    INode *y = x->right; // get right child
    
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
    
    // Update minSubtree values after rotation
    updateMinSubtree(x);
    updateMinSubtree(y);
    
    // Update parent minSubtree 
    if (y->parent != NULL) {
        updateMinSubtree(y->parent);
    }
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
    
    INode *x = y->left;  // get left child
    
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
    
    // Update minSubtree values after rotation
    updateMinSubtree(y);
    updateMinSubtree(x);
    
    // Update parent minSubtree
    if (x->parent != NULL) {
        updateMinSubtree(x->parent);
    }
}

/**
 * @brief Performs rotations and recoloring to maintain red-black properties after inserting a new node
 *
 * @param m Pointer to the MAGIC instance
 * @param newNode The newly inserted node
 */
static void rbInsertFixup(MAGIC m, INode *newNode) {
    if (m == NULL || newNode == NULL) return;

    while (newNode != m->root && newNode->parent != NULL && newNode->parent->color == RED) {
        if (newNode->parent == newNode->parent->parent->left) {
            // Case: Parent is a left child of grandparent
            INode *uncle = newNode->parent->parent->right;

            if (uncle != NULL && uncle->color == RED) {
                // Case 1: Uncle is RED -> Recolor
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent; // Move up to grandparent
            } else {
                // Case 2: Uncle is BLACK (or NULL)
                if (newNode == newNode->parent->right) {
                    // Case 2a: newNode is a right child -> Left-rotate parent
                    newNode = newNode->parent;
                    leftRotate(m, newNode);
                }
                // Case 3: Recolor and right-rotate grandparent
                newNode->parent->color = BLACK;
                newNode->parent->parent->color = RED;
                rightRotate(m, newNode->parent->parent);
            }
        } else {
            // Symmetric case: Parent is a right child of grandparent
            INode *uncle = newNode->parent->parent->left;

            if (uncle != NULL && uncle->color == RED) {
                // Case 1: Uncle is RED -> Recolor
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent; // Move up to grandparent
            } else {
                // Case 2: Uncle is BLACK (or NULL)
                if (newNode == newNode->parent->left) {
                    // Case 2a: newNode is a left child -> Right-rotate parent
                    newNode = newNode->parent;
                    rightRotate(m, newNode);
                }
                // Case 3: Recolor and left-rotate grandparent
                newNode->parent->color = BLACK;
                newNode->parent->parent->color = RED;
                leftRotate(m, newNode->parent->parent);
            }
        }
    }
    // Ensure root is always black
    if (m->root != NULL) {
        m->root->color = BLACK;
    }
}

/**
 * @brief Insert a node into the tree (ordered by sequence number)
 *
 * @param m Pointer to the MAGIC instance
 * @param newNode The new node
 */
static void rbInsert(MAGIC m, INode *newNode) {
    if (m == NULL || newNode == NULL) return;

    INode *y = NULL;
    INode *x = m->root;

    // Find the insertion point based on sequence number for chronological ordering
    while (x != NULL) {
        y = x;
        
        // Traverse based on sequence number
        if (newNode->seqNumber < x->seqNumber) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    // Set parent
    newNode->parent = y;

    // Insert node
    if (y == NULL) {
        // Tree is empty
        m->root = newNode;
    } else if (newNode->seqNumber < y->seqNumber) {
        y->left = newNode;
    } else {
        y->right = newNode;
    }
    
    // Update minSubtree for the parent
    if (y != NULL) {
        updateMinSubtree(y);
    }
    
    // Fix Red-Black properties
    rbInsertFixup(m, newNode);
    
    m->size++;
}

/**
 * @brief Process operations in order to map position from input to output
 * With pruning using minSubtree
 * 
 * @param node Current node in traversal
 * @param pos Position to map
 * @return Mapped position or -1 if invalid (or no mapping)
 */
static int mapInOut(INode *node, int pos) {
    if (node == NULL) {
        return pos; // Base case: no more operations 
    }
    
    // If position is already negative, return answer query to -1
    if (pos == -1) {
        return -1;
    }
    
    // Pruning: If position is less than minSubtree of left subtree, 
    // we can skip the entire left subtree as no operations there will affect this position
    int leftResult;
    if (node->left != NULL && pos < node->left->minSubtree) {
        // Skip left subtree
        leftResult = pos;
    } else {
        // Process left subtree
        leftResult = mapInOut(node->left, pos);
    }
    
    // If position has been marked as invalid by the left subtree, propagate it in order to stop traversal
    if (leftResult == -1) {
        return -1;
    }
    
    // Apply current operation
    int cumulativeResult = leftResult; // track cumulative shifts 
    if (node->opType == ADD) { // Add operation
        // If position is at or after insertion point, shift it
        if (node->low <= cumulativeResult) {
            cumulativeResult += (node->high - node->low);
        }
    } else { // Remove operation
        // Check if position falls within removed region
        if (node->low <= cumulativeResult && cumulativeResult < node->high) {
            return -1; // Position was removed, invalid mapping
        }
        
        // If position is after removal point, shift it back
        else if (cumulativeResult >= node->high) {
            cumulativeResult -= (node->high - node->low);
        }
    }
    
    // Pruning: If cumulativeResult is less than minSubtree of right subtree,
    // we can skip the entire right subtree
    if (node->right != NULL && cumulativeResult < node->right->minSubtree) {
        // Skip right subtree
        return cumulativeResult;
    } else {
        // Process right subtree
        return mapInOut(node->right, cumulativeResult);
    }
}

/**
 * @brief Process operations in reverse order to map position from output to input
 * With pruning using minSubtree
 * 
 * @param node Current node in traversal
 * @param pos Position to map
 * @return Mapped position or -1 if invalid
 */
static int mapOutIn(INode *node, int pos) {
    if (node == NULL) {
        return pos; // Base case: no more operations
    }
    
    // If position is already negative, propagate the invalid state
    if (pos == -1) {
        return -1;
    }
    
    // Pruning: Skip right subtree if position is less than the minSubtree of right
    int rightResult;
    if (node->right != NULL && pos < node->right->minSubtree) {
        // Skip right subtree
        rightResult = pos;
    } else {
        // Process right subtree
        rightResult = mapOutIn(node->right, pos);
    }
    
    // If position has been marked as invalid by the right subtree, propagate it to stop traversal
    if (rightResult == -1) {
        return -1;
    }
    
    // Apply current operation (in reverse)
    int cumulativeResult = rightResult;
    if (node->opType == ADD) { // Undo an add operation
        // If position is within added range, it doesn't exist in input
        if (node->low <= cumulativeResult && cumulativeResult < node->high) {
            return -1;
        }
        
        // If position is after the added range, shift backward
        if (cumulativeResult >= node->high) {
            cumulativeResult -= (node->high - node->low);
        }
    } else { // Undo a remove operation
        // If position is at or after the start of removed section,
        // shift forward by the length of the removed section
        if (node->low <= cumulativeResult) {
            cumulativeResult += (node->high - node->low);
        }
    }
    
    // Pruning: Skip left subtree if position is less than the minSubtree of left
    if (node->left != NULL && cumulativeResult < node->left->minSubtree) {
        // Skip left subtree
        return cumulativeResult;
    } else {
        // Process left subtree
        return mapOutIn(node->left, cumulativeResult);
    }
}
