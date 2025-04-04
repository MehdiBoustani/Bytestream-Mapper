#include <stdio.h>
#include <stdlib.h>
#include "magic.h"

/**
 * INFO0027: - Programming Techniques (Algorithmics)
 *  Project 1: Bytestream mapper
 * 
 * \file magic.c
 * \brief Implementation of MAGIC ADT
 * \author Boustani Mehdi -- Albashityalshaier Abdelkader (Optimized by Claude)
 * \version 1.0
 * \date 04/04/2025
 *
 * Implements the MAGIC ADT using an Interval Tree based on a Red-Black Tree
 * Sorted by sequence number with interval metadata for efficient pruning
 * 
 */

/* Opaque Structure for Interval Node */
typedef struct INode_t INode;

/* Enum for tracking colors */
typedef enum {RED, BLACK} Color;

struct INode_t {
    unsigned int low;      // lower boundary of the interval (pos)
    unsigned int high;     // high boundary of the interval (pos + length)
    unsigned int seqNumber;// sequence number for chronological ordering
    int opType;            // 1 for add, -1 for remove
    Color color;
    INode *left, *right, *parent;
};

struct magic {
    INode *root;
    size_t size;           // store number of nodes (operations)
};

/* Prototypes of static functions */
static INode *createNode(int low, int high, int opType, unsigned int seqNumber);
static void destroyTree(INode *root);
static void leftRotate(MAGIC m, INode *x);
static void rightRotate(MAGIC m, INode *x);
static void rbInsertFixup(MAGIC m, INode *z);
static void rbInsert(MAGIC m, INode *z);
static void inOrderTraversal(INode *root, INode ***operations, int *count);
static int inOutMap(INode **operations, int opCount, int pos);
static int outInMap(INode **operations, int opCount, int pos);

/* Static Functions */

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
static INode *createNode(int low, int high, int opType, unsigned int seqNumber) {
    if (low < 0 || high < 0 || low > high) {
        printf("Invalid interval boundaries\n");
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
    n->color = RED;     // New nodes are always RED in a Red-Black Tree
    n->parent = NULL;
    n->left = NULL;
    n->right = NULL;
    
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
}

/**
 * @brief Performs rotations and recoloring to maintain red-black properties after inserting a new node
 *
 * @param m Pointer to the MAGIC instance
 * @param z The newly inserted node
 */
static void rbInsertFixup(MAGIC m, INode *z) {
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
                // Case 2: Uncle is BLACK (or NULL)
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
                // Case 2: Uncle is BLACK (or NULL)
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
    if (m->root != NULL) {
        m->root->color = BLACK;
    }
}

/**
 * @brief Insert a node into the tree (ordered by sequence number)
 *
 * @param m Pointer to the MAGIC instance
 * @param z The new node
 */
static void rbInsert(MAGIC m, INode *z) {
    if (m == NULL || z == NULL) return;

    INode *y = NULL;
    INode *x = m->root;

    // Find the insertion point based on sequence number for chronological ordering
    while (x != NULL) {
        y = x;
        
        // Traverse based on sequence number
        if (z->seqNumber < x->seqNumber) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    // Set parent
    z->parent = y;

    // Insert node
    if (y == NULL) {
        // Tree is empty
        m->root = z;
    } else if (z->seqNumber < y->seqNumber) {
        y->left = z;
    } else {
        y->right = z;
    }
    
    // Fix Red-Black properties
    rbInsertFixup(m, z);
}

/**
 * @brief Perform an in-order traversal of the tree to collect operations
 *
 * @param root Root node of the tree
 * @param operations Pointer to array of operations
 * @param count Pointer to count of operations
 */
static void inOrderTraversal(INode *root, INode ***operations, int *count) {
    if (root == NULL) return;
    
    // Traverse left subtree
    inOrderTraversal(root->left, operations, count);
    
    // Process current node
    (*operations)[(*count)++] = root;
    
    // Traverse right subtree
    inOrderTraversal(root->right, operations, count);
}

/**
 * @brief Map a position from input stream to output stream
 *
 * @param operations Array of operations in sequence order
 * @param opCount Number of operations
 * @param pos Position to map
 * 
 * @return Mapped position or -1 if invalid
 */
static int inOutMap(INode **operations, int opCount, int pos) {
    int result = pos;
    
    for (int i = 0; i < opCount; i++) {
        INode *op = operations[i];
        
        if (op->opType == 1) { // Add operation
            // If position is at or after insertion point, shift it
            if (op->low <= result) {
                result += (op->high - op->low);
            }
        } else { // Remove operation
            // Check if position falls within removed region
            if (op->low <= result && result < op->high) {
                return -1; // Position was removed, invalid mapping
            }
            
            // If position is after removal point, shift it back
            if (result >= op->high) {
                result -= (op->high - op->low);
            }
        }
    }
    
    return result;
}

/**
 * @brief Map a position from output stream to input stream
 *
 * @param operations Array of operations in sequence order
 * @param opCount Number of operations
 * @param pos Position to map
 * 
 * @return Mapped position or -1 if invalid
 */
static int outInMap(INode **operations, int opCount, int pos) {
    int result = pos;
    
    // Process operations in reverse order
    for (int i = opCount - 1; i >= 0; i--) {
        INode *op = operations[i];
        
        if (op->opType == 1) { // Undo an add operation
            // If position is within added range, it doesn't exist in input
            if (op->low <= result && result < op->high) {
                return -1;
            }
            
            // If position is after the added range, shift backward
            if (result >= op->high) {
                result -= (op->high - op->low);
            }
        } else { // Undo a remove operation
            // If position is at or after the start of removed section,
            // shift forward by the length of the removed section
            if (op->low <= result) {
                result += (op->high - op->low);
            }
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

void MAGICadd(MAGIC m, int pos, int length) {
    if (m == NULL || length <= 0 || pos < 0)
        return;

    INode *newNode = createNode(pos, pos + length, 1, m->size);
    if (newNode == NULL)
        return;

    rbInsert(m, newNode);
    m->size++;
}

void MAGICremove(MAGIC m, int pos, int length) {
    if (m == NULL || length <= 0 || pos < 0)
        return;

    INode *newNode = createNode(pos, pos + length, -1, m->size);
    if (newNode == NULL)
        return;

    rbInsert(m, newNode);
    m->size++;
}

int MAGICmap(MAGIC m, MAGICDirection direction, int pos) {
    if (m == NULL || pos < 0)
        return -1;
    
    if (m->root == NULL) 
        return pos; // No operations, mapping is identity
    
    // 1. Allocate array to store operations
    INode **operations = malloc(m->size * sizeof(INode *));
    if (operations == NULL) {
        printf("MAGICmap: Allocation error\n");
        return -1;
    }
    
    // 2. Collect operations in sequence order
    int opCount = 0;
    inOrderTraversal(m->root, &operations, &opCount);
    
    // 3. Map position according to direction
    int result;
    if (direction == STREAM_IN_OUT) {
        result = inOutMap(operations, opCount, pos);
    } else { // STREAM_OUT_IN
        result = outInMap(operations, opCount, pos);
    }
    
    // 4. Clean up and return
    free(operations);
    return result;
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