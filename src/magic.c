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
    Color color; 
    INode *left, *right, *parent;
};

struct magic {
    INode *root;
    size_t size; // store number of nodes (!!!! NOT size of bytestream)
};

/* Prototypes of static functions */
static INode *newINode(int low, int high);
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
static INode *newINode(int low, int high) {
    INode *n = malloc(sizeof(INode));
    if (n == NULL) {
        printf("newINode: Allocation error\n");
        return NULL;
    }

    n->low  = low;
    n->high = high;
    n->maxEnd = high;
    n->color = RED; // by default
    n->parent = NULL;
    n->left   = NULL;
    n->right  = NULL;

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


// A AJUSTER !!!!
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
            // new node ha
            x = x->left;
        } else {
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


// A REFAIRE !!!
void MAGICadd(MAGIC m, int pos, int length){
    if (m == NULL || length <= 0)
        return;

    INode *newNode = newINode(pos, pos + 1, length);
    if (newNode == NULL)
        return;

    rbInsert(m, newNode);

    m->size++;
}

void MAGICremove(MAGIC m, int pos, int length){
    // TODO
}

int MAGICmap(MAGIC m, MAGICDirection direction, int pos){
    // TODO
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