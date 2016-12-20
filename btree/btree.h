/*
 * file: btree.h
 * author: David Harmon
 * date: 10/17/16
 *
 * Function stubs for the b-tree implementation.
 */

/* root node */
typedef struct btNode *bTree;

/* btree node comparison function */
typedef int (NodeCompareFunction)(int element, int key);

/* create a new initially empty tree */
bTree* btCreate();

/* destrioy a tree, free all memory associated with it */
void btDestroy(bTree* bt);

/* searches tree for given key, returns null if not present */
int btSearch(bTree* bt, int key, NodeCompareFunction* fn);

/* inserts a new element into a given tree, returns tree if this created a
 * new node, false if it just appended to another node */
int btInsert(bTree* bt, int key, NodeCompareFunction* fn);

/* print the structure of the b-tree in a manner readable by humans
 * basically, print keys in a tree-like manner */
void btPrint(bTree *bt);
