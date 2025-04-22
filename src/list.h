#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct List {
    void *items;
    size_t cap : 32;
    size_t len : 32;
} List;

List list_init_sized(size_t item_size, size_t init_cap);
#define list_init(T, init_cap) list_init_sized(sizeof(T), init_cap)

void list_deinit(List *ls);

void list_push_sized(List *ls, size_t item_size, void *item);
#define list_push(ls, T, item) list_push_sized(ls, sizeof(T), &(T){ item })
#define list_push_addr(ls, T, item) list_push_sized(ls, sizeof(T), item)

void *list_get_sized(List *ls, size_t item_size, size_t idx);
#define list_get(ls, T, idx) (*(T*)list_get_sized(ls, sizeof(T), idx))
#define list_get_ref(ls, T, idx) ((T*)list_get_sized(ls, sizeof(T), idx))

#endif
