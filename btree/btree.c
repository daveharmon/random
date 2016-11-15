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
    bt->fn = fn;

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

/* return smallest index i in sorted array such that key <= a[i]
 * (or n if there is no such index) */
static int search_key(int n, const int *a, void *key, NodeCompareFunction* fn)
{
    int lo;
    int hi;
    int mid;

    /* invariant: a[lo] < key <= a[hi] */
    lo = -1;
    hi = n;

    while(lo + 1 < hi) {
        mid = (lo+hi)/2;
        if(0 == *fn(a[mid], key)) {
            return mid;
        } else if(-1 == *fn(a[mid], key)) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return hi;
}

/* searches tree for given key, returns NULL if not present */
void* btSearch(bTree* bt, void *key, NodeCompareFunction* fn)
{
    if (fn == NULL)
        fn = &default_comparison_function;

    if (0 != bt->filled_keys){
        int pos = search_key(bt->filled_keys, bt->keys, key, fn);

        if (pos < b->filled_keys && 0 == fn(bt->keys[pos], key))
            return bt->keys[pos];
        else if (!bt->is_leaf)
            return btSearch(bt->children[pos], key, fn);
    }
    return NULL;
}

/*
 * inserts a new key into the tree at an existing node
 * returns the right sibling if this causes the node to split,
 * puts median key in *median
 */
static *bTree bt_insert_internal(bTree *bt, void *key, void *median,
    NodeCompareFunction* fn)
{
    int pos = search_key(bt->filled_keys, bt->keys, key, fn);
    int mid;
    bTree *right;

    /* if the key doesn't already exist */
    if (pos == bt->filled_keys) {
        if (bt->is_leaf) {
            /* everybody above pos moves up one space */
            memmove(bt->keys[pos+1], bt->keys[pos], sizeof(*(bt->keys)) *
                (bt->numKeys - pos));
            bt->keys[pos] = key;
            bt->filled_keys++;
        } else {
            /* insert in child */
            right = bt_insert_internal(bt, key, &mid, fn);

            /* we may need to insert a new key into the subtree */
            if (right) {
                /* every key above pos moves up one space */
                memmove(bt->keys[pos+1], bt->keys[pos], sizeof(*(bt->keys))
                    * (bt->filled_keys - pos));
                /* new kid goes in pos + 1 */
                memmove(bt->kids[pos+2], bt->kids[pos+1], sizeof(*(bt->keys))
                    * (bt->filled_keys - pos));

                b->keys[pos] = mid;
                b->children[pos+1] = right;
                b->filled_keys++;
            }
        }

        /* we waste a little space by splitting now rather than on next insert */
        if (b->filled_keys >= MAX_KEYS) {
            mid = bt->filled_keys/2;

            median = bt->keys[mid];
        }
    }

    return NULL;
}

/* inserts a new element into a given tree, returns tree if this created a
 * new node, false if it just appended to another node */
int btInsert(bTree* bt, void *key, NodeCompareFunction* fn)
{
    if (fn == NULL)
        fn = &default_comparison_function;

    bTree *left;   /* new left child */
    bTree *right;   /* new right child */
    int median;

    right = btInsertInternal(bt, key, &median, fn);

    if(right) {
        /* basic issue here is that we are at the root
         * so if we split, we have to make a new root */

        left = malloc(sizeof(bTree));
        assert(left);

        /* copy root to b1 */
        memmove(left, bt, sizeof(*bt));

        /* make root point to b1 and b2 */
        bt->filled_keys = 1;
        bt->is_leaf = 0;
        bt->keys[0] = median;
        bt->children[0] = left;
        bt->children[1] = right;
    }
}

/* print the structure of the b-tree in a manner readable by humans
 * basically, print keys in a tree-like manner */
void btPrint(bTree *bt)
{

}
