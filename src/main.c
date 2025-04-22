#include "module.h"
#include "program.h"

int main(void) {
    Program prg = program_init("/src");

    program_compile(&prg);

    print_ast(stderr, list_get_ref(&prg.mods, Module, 0)->ast);

    return 0;
}
