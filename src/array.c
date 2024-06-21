#include "array.h"

#include <stdlib.h>
#include <string.h>

typedef Array(void) ArrayVoid;

static void reserve_one(void *anyarr, size_t size) {
    ArrayVoid *arr = (ArrayVoid*)anyarr;

    if (arr->len + 1 > arr->cap) {
        arr->cap = (size_t)((float)arr->cap * 1.5f) + 8;
        arr->ptr = realloc(arr->ptr, size * arr->cap);
    }
}

void array_priv_init(void *anyarr, size_t size, size_t init_cap) {
    ArrayVoid *arr = (ArrayVoid*)anyarr;

    arr->ptr = malloc(size * init_cap);
    arr->cap = init_cap;
    arr->len = 0;
}

void array_deinit(void *anyarr) {
    ArrayVoid *arr = (ArrayVoid*)anyarr;

    free(arr->ptr);
    arr->len = 0;
    arr->cap = 0;
}

void array_priv_push(void *anyarr, size_t size, void *elem) {
    ArrayVoid *arr = (ArrayVoid*)anyarr;

    reserve_one(arr, size);

    memcpy((unsigned char*)arr->ptr + (size * arr->len), elem, size);
    arr->len += 1;
}

void array_pop(void *anyarr) {
    ArrayVoid *arr = (ArrayVoid*)anyarr;

    arr->len -= 1;
}

void *array_priv_add(void *anyarr, size_t size) {
    ArrayVoid *arr = (ArrayVoid*)anyarr;

    reserve_one(arr, size);

    void *new = (unsigned char*)arr->ptr + (size * arr->len);
    arr->len += 1;

    return new;
}

void *array_priv_get(void *anyarr, size_t size, size_t idx) {
    ArrayVoid *arr = (ArrayVoid*)anyarr;

    return (unsigned char*)arr->ptr + (size * idx);
}

