#ifndef AST_H
#define AST_H

#include "span.h"
#include "array.h"
#include <stdbool.h>


typedef enum AstTypeKind {
    AstType_Symbol,
    AstType_Int,
} AstTypeKind;

typedef struct AstType {
    AstTypeKind kind;
    Span span;
} AstType;

typedef struct AstFuncParam {
    Span     name;
    AstType *type;
} AstFuncParam;

typedef struct AstFuncSig {
    Array(AstFuncParam) params;
    AstType            *ret_type;
} AstFuncSig;

typedef struct AstFunc {
    AstFuncSig sig;
} AstFunc;

typedef enum AstItemKind {
    AstItem_FuncDef,
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
