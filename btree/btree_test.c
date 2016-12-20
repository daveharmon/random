#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "btree.h"

int main(int argc, char **argv)
{
    bTree *b;
    int i;

    b = btCreate();
    assert(b);

    assert(btSearch(b, 12, NULL) == 0);
    btInsert(b, 12, NULL);
    assert(btSearch(b, 12, NULL) == 1);
    btDestroy(b);

    b = btCreate();
    for(i = 0; i < 100; i++) {
        assert(btSearch(b, i, NULL) == 0);
        btInsert(b, i, NULL);
        assert(btSearch(b, i, NULL) == 1);
    }

    for(i = 0; i < 100; i++) {
        assert(btSearch(b, i, NULL) == 1);
        btInsert(b, i, NULL);
        assert(btSearch(b, i, NULL) == 1);
    }

    btDestroy(b);

    b = btCreate();
    for(i = 0; i < 10000000; i += 2) {
        assert(btSearch(b, i, NULL) == 0);
        btInsert(b, i, NULL);
        assert(btSearch(b, i+1, NULL) == 0);
        assert(btSearch(b, i, NULL) == 1);
    }

    btDestroy(b);

    return 0;
}
