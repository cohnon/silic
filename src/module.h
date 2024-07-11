#ifndef MODULE_H
#define MODULE_H

#include "ast.h"
#include "dynarr.h"
#include "namespace.h"
#include "token.h"


typedef struct Module {
    FirString           file_path;
    FirString           src;

    DynArr(Token)    tokens;

    DynArr(AstItem*) uses;
    DynArr(AstItem*) ast;

    Namespace        ns;
} Module;

Module *module_init(FirString file_path);
void tok_debug(Module *module);

#endif
