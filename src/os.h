#ifndef OS_H
#define OS_H

#include "span.h"
#include <stdbool.h>

#define os_alloc_T(T) (T*)os_priv_alloc(sizeof(T))
#define os_alloc(T, size) (T*)os_priv_alloc(sizeof(T) * size)
void *os_priv_alloc(size_t size);

typedef struct OsReadFileResult {
    bool ok;
    Span src;
} OsReadFileResult;
OsReadFileResult os_read_file(Span path);

#endif
