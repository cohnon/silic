#ifndef MODULE_H
#define MODULE_H

#include "array.h"
#include "ast.h"
#include "error_msg.h"
#include "span.h"
#include "token.h"


typedef struct Module {
    Span            src;
    Array(Token)    tokens;
    Array(AstItem*)  ast;

    Array(ErrorMsg) errors;
} Module;

Module *module_init(Span src);
void tok_debug(Module *module);
void ast_debug(Module *module);

#endif
