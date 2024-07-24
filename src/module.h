#ifndef MODULE_H
#define MODULE_H

#include "ast.h"
#include "token.h"

#include <fir/dynarr.h>
#include <fir/os.h>

typedef struct Module Module;

typedef struct Module {
    String               file_path;
    String               src;

    DynArr(Token)        tokens;

    DynArr(AstItem*)     ast;
} Module;

Module *module_init(String file_path, String source);
void tok_debug(Module *module);

#endif
