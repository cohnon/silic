#include "program.h"
#include "module.h"

Program program_init(char *root_mod_path) {
    Program prg;
    prg.mods = list_init(Module, 8);
    prg.root_mod_path = root_mod_path;

    return prg;
}

void program_compile(Program *prg) {
    char *src =
        "let add (a:I32, b:I32) -> I32 = a + b\n"
    ;

    Module root_mod = module_init(src);
    module_parse(&root_mod);

    list_push_addr(&prg->mods, Module, &root_mod);
}
