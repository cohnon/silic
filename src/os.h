#ifndef OS_H
#define OS_H

#include "span.h"
#include <stdbool.h>

#define os_alloc(T) (T*)os_priv_alloc(sizeof(T))
void *os_priv_alloc(size_t size);

bool os_read_file(Span *out, Span path);

#endif
