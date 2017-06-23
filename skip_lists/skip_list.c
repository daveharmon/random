/*
 *
 */

#include <stdlib.h>
#include <limits.h>


typedef struct snode_t
{
    void* key;
    void* value;
    struct snode_t** forward;
} snode_t;


#define SKIPLIST_MAX_LEVEL 7

static inline snode_t*
init_snode(snode_t* node, void* key, void* value, int level)
{
    node = malloc(sizeof(snode_t));
    node->key = key;
    node->value = value;
    node->forward = malloc(sizeof(snode_t*) * (level + 1));
}


slist_t*
skiplist_init(slist_t *list)
{
    int i;
    snode_t *header = malloc(sizeof(snode_t));
    list->header = header;
    header->key = INT_MAX;
    header->forward = malloc(sizeof(snode_t*) * (SKIPLIST_MAX_LEVEL));
    for (i = 0; i < SKIPLIST_MAX_LEVEL; i++) {
        header->forward[i] = list->header;
    }

    list->level = 1;
    list->size = 0;

    return list;
}

static int
rand_level()
{
    int level = 1;
    while (rand() < RAND_MAX/2 && level < SKIPLIST_MAX_LEVEL)
        level++;
    return level;
}

int
search(slist_t* list, void* search_key)
{
    if (!list || !list->header)
        return -1;

    snode_t x = list->header;    // loop invariant: x->key < search_key
    for (unsigned int i = list->level; i > 0; i--) {
        while (x->forward[i]->key < search_key) {
            x = x->forward[i];
        }   // x->key < search_key <= x->forward[1]->key
    }

    x = x->forward[1];
    if (x->key == search_key)
        return x->value;

    return -1;
}

int
insert(slist_t* list, void* search_key, void* new_value)
{
    if (!list || !list->header)
        return -1;

    int i, new_level;

    snode_t* update[SKIPLIST_MAX_LEVEL];
    snode_t* x = list->header;

    for (i = list->level; i > 0; i--) {
        while (x->forward[i]->key < search_key)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[1];

    if (x->key == search_key){
        x->value = new_value;
    } else {
        new_level = rand_level();
        if (new_level > list->level) {
            for (i = list->level+1; i < new_level; i++)
                update[i] = list->header;
            list->level = new_level
        }
        x = init_snode(x, search_key, new_value, new_level);
        for (i = 1; i < new_level; i++) {
            x->forward[i] = update[i]->forward;
            update[i]->forward[i] = x;
        }
    }
    return 0;
}
