#ifndef MODULE_H
#define MODULE_H

#include "ast.h"
#include "dynarr.h"
#include "error_msg.h"
#include "namespace.h"
#include "token.h"


typedef struct Module {
    FirString           filepath;
    FirString           src;

    DynArr(Token)    tokens;

    DynArr(AstItem*) uses;
    DynArr(AstItem*) ast;

    Namespace        ns;

    DynArr(ErrorMsg) errors;
} Module;

Module *module_init(FirString src);
void tok_debug(Module *module);

#endif
