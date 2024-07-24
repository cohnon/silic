#ifndef NAMERES_H
#define NAMERES_H

// the name resolution phase of the compiler links all symbols to their
// definitions in the AST.

#include "compiler.h"
#include <fir/map.h>


typedef enum AstDefKind {
    AstDef_Mod,
    AstDef_Var,
    AstDef_Func,
} AstDefKind;

typedef struct AstDef {
    AstDefKind kind;

    union {
        AstExpr *expr;
        AstItem *item;
    };
} AstDef;

typedef struct Namespace Namespace;
typedef struct Namespace {
    Namespace   *parent;
    Map(AstDef) symbols;
} Namespace;

bool resolve_names(Compiler *compiler);

#endif
