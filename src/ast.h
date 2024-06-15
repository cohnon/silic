#ifndef AST_H
#define AST_H

#include "span.h"
#include "array.h"
#include <stdbool.h>


typedef enum AstTypeKind {
    AstType_Symbol,
} AstTypeKind;

typedef struct AstType {
    AstTypeKind kind;
    Span span;
} AstType;

typedef struct AstFuncSig {
    Array(AstType) param_types;
    Array(Span)    param_names;
    AstType        ret_type;
} AstFuncSig;

typedef struct AstFunc {
    AstFuncSig sig;
} AstFunc;

typedef enum AstItemKind {
    AstItem_Func,
} AstItemKind;

typedef struct AstItem {
    AstItemKind kind;
    bool public;

    Span name;

    union {
        AstFunc func;
    };
} AstItem;

#endif
