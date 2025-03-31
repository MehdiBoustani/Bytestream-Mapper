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
    int low;     // lower boundary of the interval
    int high;    // high boundary of the interval 
    int maxEnd;  // max endpoint in the subtree
    int offset;  // A VOIR SI ON GARDE
    Color color; 
    INode *left, *right, *parent;
};

struct magic {
    INode *root;
};

/* Prototypes of static functions */
static INode *newINode(int low, int high, int offset);
static void leftRotate(MAGIC m, INode *rotNode);
static void rightRotate(MAGIC m, INode *rotNode);


/* Static Functions */

/**
 * @brief Create a new Interval Tree node with a given range.
 *
 * @param low low value of the interval
 * @param high high value of the interval
 * 
 * @return INode* a pointer to the new created Interval Node
 */
static INode *newINode(int low, int high, int offset) {
    INode *n = malloc(sizeof(INode));
    if (n == NULL) {
        printf("newINode: Allocation error\n");
        return NULL;
    }

    n->low  = low;
    n->high = high;
    n->maxEnd = high;
    n->offset = offset;
    n->color = RED; // by default
    n->parent = NULL;
    n->left   = NULL;
    n->right  = NULL;

    return n;
}

// A REVOIR !!!!!!!!!!!!!!

/**
 * @brief Performs a left rotation at a given node
 * 
 * @param magic Pointer to the MAGIC instance
 * @param rotNode The node to rotate around
 */
static void leftRotate(MAGIC m, INode *rotNode){
    if (m == NULL || rotNode == NULL || rotNode->right == NULL)  {
        printf("leftRotate: NULL Error\n");
        return;
    }

    INode *n = rotNode->right; // pick the right node 

    // Turn n's left subtree into rotNode's right subtree
    rotNode->right = n->left;
    if (n->left != NULL) {
        n->left->parent = rotNode;
    }

    n->parent = rotNode->parent;
    if (rotNode->parent == NULL) {
        // node was the root
        m->root = n; 
    } else if (rotNode = rotNode->parent->left) {
        n->parent->left = n;
    } else {
        rotNode->parent->right = n;
    }

    n->left = rotNode;
    rotNode->parent = n;

    // Update maxEnd
    n->maxEnd = n->high;
    if (n->left != NULL && n->left->maxEnd > n->maxEnd) {
        n->maxEnd = n->left->maxEnd;
    }
    if (n->right != NULL && n->right->maxEnd > n->maxEnd) {
        n->maxEnd = n->right->maxEnd;
    }
}

static void rightRotate(MAGIC magic, INode *rotNode){
    // TODO
}

/* Implementation of API */

MAGIC MAGICinit() {
    MAGIC m = malloc(sizeof(struct magic));
    if (m == NULL) {
        printf("MAGICinit: Allocation error\n");
        return NULL;
    }

    m->root = NULL;
    return m;
}

void MAGICadd(MAGIC m, int pos, int length){
    // TODO
}

void MAGICremove(MAGIC m, int pos, int length){
    // TODO
}

int MAGICmap(MAGIC m, MAGICDirection direction, int pos){
    // TODO
}

void MAGICdestroy(MAGIC m){
    // TODO
}