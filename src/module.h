#ifndef MODULE_H
#define MODULE_H

#include "token.h"
#include "span.h"
#include "array.h"


typedef struct Module {
    Span         src;
    Array(Token) tokens;
} Module;

#endif
