#include "list.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

List list_init_sized(size_t item_size, size_t init_cap) {
    List ls;
    ls.items = malloc(item_size * init_cap);
    ls.cap = init_cap;
    ls.len = 0;

    return ls;
}

void list_deinit(List *ls) {
    free(ls->items);
    ls->items = 0;
    ls->cap = 0;
    ls->len = 0;
}

void list_push_sized(List *ls, size_t item_size, void *item) {
    if (ls->len + 1 > ls->cap) {
        ls->cap = (size_t)((float)ls->cap * 1.5);
        ls->items = realloc(ls->items, ls->cap * item_size);
    }

    memcpy((unsigned char *)ls->items + (item_size * ls->len), item, item_size);
    ls->len += 1;
}

void *list_get_sized(List *ls, size_t item_size, size_t idx) {
    assert(idx < ls->len);

    return (unsigned char *)ls->items + (item_size * idx);
}
