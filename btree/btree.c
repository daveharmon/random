/*
 * file: btree.c
 * author: David Harmon
 * date: 10/17/16
 *
 * A simple implementation of the b-tree balanced n-ary tree data structure.
 * A b-tree is a complete tree commonly used to represent large structures
 * such as filesystems on disk.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "btree.h"

#define MAX_KEYS (1024)

typedef struct btNode {
    int is_leaf;     /* if true, then this will have no children */
    int filled_keys; /* the number of keys out of MAX_KEYS that are used */
    void *keys[MAX_KEYS];   /* the keys associated with this node */
    struct btNode *children[MAX_KEYS + 1]; /* children[i] holds nodes < keys[i] */
} btNode;

/* default comparison function, works on ints */
int default_comparison_function(void *i1, void *i2)
{
    if ((int) *i1 < (int) *i2) return -1;
    else if ((int) *i1 > (int) *i2) return 1;
    else if ((int) *i1 == (int) *i2) return 0;

    printf("comparison failure\n");
    return -2;
}

/* create a new initially empty tree */
bTree* btCreate()
{
    bTree bt = (bTree *) malloc(sizeof(bTree));
    assert(bt);

    bt->is_leaf = 1;
    bt->filled_keys = 0;

    return bt;
}

/* destrioy a tree, free all memory associated with it */
void btDestroy(bTree* bt)
{
    if (!bt->is_leaf) {
        for (int i = 0; i < bt->filled_keys; i++)
            btDestroy(bt->children[i]);
    }

    free(bt);
}

/* searches tree for given key, returns null if not present */
void* btSearch(bTree* bt, void *key, NodeCompareFunction* fn)
{
 if (fn == null)
    fn = &default_comparison_function;


}

/* inserts a new element into a given tree, returns tree if this created a
 * new node, false if it just appended to another node */
int btInsert(bTree* bt, void *key, NodeCompareFunction* fn)
{
 if (fn == null)
    fn = &default_comparison_function;


}

/* print the structure of the b-tree in a manner readable by humans
 * basically, print keys in a tree-like manner */
void btPrint(bTree *bt)
{

}
