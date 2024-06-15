#ifndef SPAN_H
#define SPAN_H

#include <stddef.h>

typedef struct Span {
    char  *ptr;
    size_t len;
} Span;

#define span_fmt(span) (int)(span).len, (span).ptr

#endif
