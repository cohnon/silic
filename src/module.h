#ifndef MODULE_H
#define MODULE_H

#include "ast.h"
#include "dynarr.h"
#include "error_msg.h"
#include "span.h"
#include "token.h"

typedef struct Module {
    FirSym           filepath;
    FirSym           src;
    DynArr(Token)    tokens;
    DynArr(AstItem*) ast;

    DynArr(ErrorMsg) errors;
} Module;

Module *module_init(FirSym src);
void tok_debug(Module *module);
void ast_debug(Module *module);

#endif
