#include "acb/acb.h"
#include "module.h"
#include "program.h"

#include <stdio.h>

Module *root_module(Program *prg) {
    return list_get_ref(&prg->mods, Module, 0);
}

int main(void) {
    Program prg = program_init("/src");

    program_compile(&prg);

    print_ast(stderr, list_get_ref(&prg.mods, Module, 0)->ast);

    FILE *fp = fopen("test.c", "w");
    acb_emit_c(fp);
    fclose(fp);

    return 0;
}
