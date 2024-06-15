#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>


#define Array(T) struct { \
    T *ptr;                \
    size_t len;            \
    size_t cap;            \
}

#define array_init(arr, init_cap) array_priv_init(arr, sizeof(__typeof__(*(arr)->ptr)), init_cap)
void array_priv_init(void *arr, size_t size, size_t init_cap);
void array_deinit(void *arr);

// assigns to tmp to warn if elem is of wrong type
#define array_push(arr, elem) do { \
    __typeof__((arr)->ptr) tmp = elem; \
    array_priv_push(arr, sizeof(__typeof__(*(arr)->ptr)), tmp); \
} while (0)
void array_priv_push(void *arr, size_t size, void *elem);

#define array_add(arr) (__typeof__( (arr)->ptr ))array_priv_add(arr, sizeof(__typeof__(*(arr)->ptr)))
void *array_priv_add(void *arr, size_t size);

#define array_get(arr, idx) *array_get_ref(arr, idx)
#define array_get_ref(arr, idx) (__typeof__( (arr)->ptr ))array_priv_get(arr, sizeof(__typeof__(*(arr)->ptr)), idx)
void *array_priv_get(void *arr, size_t size, size_t idx);

#define array_foreach(arr, i) for (size_t i = 0; i < (arr).len; i += 1)

#endif
