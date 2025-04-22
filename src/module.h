#ifndef MODULE_H
#define MODULE_H

#include "ast.h"

typedef struct Module {
    char *src;
    Ast *ast;
} Module;

Module module_init(char *src);
void module_parse(Module *mod);

#endif
