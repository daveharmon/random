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
    int keys[MAX_KEYS];   /* the keys associated with this node */
    struct btNode *children[MAX_KEYS + 1]; /* children[i] holds nodes < keys[i] */
    NodeCompareFunction* fn;
} btNode;

btNode *bt_insert_internal(bTree *bt, int key, int median,NodeCompareFunction* fn);
int bt_search(btNode *root, int key, NodeCompareFunction* fn);
void bt_destroy(btNode *root);

/* default comparison function, works on ints */
int default_comparison_function(int i1, int i2)
{
    if (i1 < i2) return -1;
    else if (i1 > i2) return 1;
    else if (i1 == i2) return 0;

    printf("comparison failure\n");
    return -2;
}

/* create a new initially empty tree */
bTree* btCreate(NodeCompareFunction* fn)
{
    btNode *bt = (btNode *) malloc(sizeof(btNode));
    assert(bt);

    bt->is_leaf = 1;
    bt->filled_keys = 0;
    bt->fn = fn;

    return (bTree *) bt;
}

/* destrioy a tree, free all memory associated with it */
void btDestroy(bTree* bt)
{
    btNode *root = (btNode *) bt;

    bt_destroy(root);

    free(root);
}

void bt_destroy(btNode *root) {
    if (!root->is_leaf) {
        for (int i = 0; i < root->filled_keys; i++)
            bt_destroy(root->children[i]);
    }
}

/* return smallest index i in sorted array such that key <= a[i]
 * (or n if there is no such index) */
static int search_key(int n, const int *a, int key, NodeCompareFunction* fn)
{
    int lo;
    int hi;
    int mid;

    if (fn == NULL)
        fn = default_comparison_function;

    /* invariant: a[lo] < key <= a[hi] */
    lo = -1;
    hi = n;

    while(lo + 1 < hi) {
        mid = (lo+hi)/2;
        if(0 == fn(a[mid], key)) {
            return mid;
        } else if(-1 == fn(a[mid], key)) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return hi;
}

/* searches tree for given key, returns NULL if not present */
int btSearch(bTree* bt, int key, NodeCompareFunction* fn)
{
    if (fn == NULL)
        fn = &default_comparison_function;

    btNode *root = (btNode *) bt;

    return bt_search(root, key, fn);
}

int bt_search(btNode *root, int key, NodeCompareFunction* fn)
{
    if (0 != root->filled_keys){
        int pos = search_key(root->filled_keys, root->keys, key, fn);

        if (pos < root->filled_keys && 0 == fn(root->keys[pos], key))
            return root->keys[pos];
        else if (!root->is_leaf)
            return bt_search(root->children[pos], key, fn);
    }
    return -1;
}

/*
 * inserts a new key into the tree at an existing node
 * returns the right sibling if this causes the node to split,
 * puts median key in *median
 */
 btNode *bt_insert_internal(bTree *bt, int key, int median,
        NodeCompareFunction* fn)
{
    int pos = search_key(bt->filled_keys, bt->keys, key, fn);
    int mid;
    btNode *right;

    /* if the key doesn't already exist */
    if (pos == bt->filled_keys) {
        if (bt->is_leaf) {
            /* everybody above pos moves up one space */
            memmove(&bt->keys[pos+1], &bt->keys[pos], sizeof(*(bt->keys)) *
                (bt->filled_keys - pos));
            bt->keys[pos] = key;
            bt->filled_keys++;
        } else {
            /* insert in child */
            right = bt_insert_internal(bt, key, mid, fn);

            /* we may need to insert a new key into the subtree */
            if (right) {
                /* every key above pos moves up one space */
                memmove(&bt->keys[pos+1], &bt->children[pos], sizeof(*(bt->keys))
                    * (bt->filled_keys - pos));
                /* new kid goes in pos + 1 */
                memmove(&bt->children[pos+2], &bt->children[pos+1], sizeof(*(bt->keys))
                    * (bt->filled_keys - pos));

                bt->keys[pos] = mid;
                bt->children[pos+1] = right;
                bt->filled_keys++;
            }
        }

        /* we waste a little space by splitting now rather than on next insert */
        if (bt->filled_keys >= MAX_KEYS) {
            mid = bt->filled_keys/2;

            median = bt->keys[mid];
        }
    }

    return NULL;
}

/* inserts a new element into a given tree, returns tree if this created a
 * new node, false if it just appended to another node */
int btInsert(bTree* bt, int key, NodeCompareFunction fn)
{
    if (fn == NULL)
        fn = default_comparison_function;

    btNode *root = (btNode *) bt;

    btNode *left;   /* new left child */
    btNode *right;   /* new right child */
    int median;

    right = bt_insert_internal(root, key, median, fn);

    if(right) {
        /* basic issue here is that we are at the root
         * so if we split, we have to make a new root */

        left = malloc(sizeof(bTree));
        assert(left);

        /* copy root to b1 */
        memmove(left, root, sizeof(btNode));

        /* make root point to b1 and b2 */
        root->filled_keys = 1;
        root->is_leaf = 0;
        root->keys[0] = median;
        root->children[0] = left;
        root->children[1] = right;
    }
}

/* print the structure of the b-tree in a manner readable by humans
 * basically, print keys in a tree-like manner */
void btPrint(bTree *bt)
{

}
