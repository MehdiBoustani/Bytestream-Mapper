#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
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
 * Implements the MAGIC ADT using an Interval Tree
 * 
 */

/* Opaque Structure for Interval Node */
typedef struct INode_t INode;

struct INode_t
{
    int low;
    int high;
    int maxEnd;  // max endpoint in the subtree
    INode *left, *right;
};

/* Prototypes of static functions */
static INode *newINode(int low, int high);


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
    n->left  = NULL;
    n->right = NULL;

    return n;
}

