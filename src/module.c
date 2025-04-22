#include "module.h"
#include "fe/parser.h"

Module module_init(char *src) {
    Module mod;
    mod.src = src;
    mod.ast = NULL;

    return mod;
}

void module_parse(Module *mod) {
    mod->ast = parser_parse(mod->src);
}
