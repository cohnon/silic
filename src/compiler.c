#include "compiler.h"

#include "lexer.h"
#include "os.h"
#include "parser.h"
#include "try.h"
#include <stdio.h>
#include <string_builder.h>


Compiler *compiler_init(void) {
    Compiler *compiler = os_alloc_T(Compiler);

    ns_init(&compiler->ns, NULL);
    dynarr_init(&compiler->modules, 4);

    return compiler;
}

static Module *add_module(Compiler *compiler, FirString file_path) {
    Module *module = try (module_init(file_path));

    try_else (lex_module(module), error_print(module));

    // tok_debug(module);

    try_else (parse_module(module), error_print(module));

    ast_debug(module);

    dynarr_foreach(module->uses, i) {
        AstItem *item = dynarr_get(&module->uses, i);

        StringBuilder use_path_b = sb_init(64);
        sb_printf(&use_path_b, "%.*s", fir_string_fmt(compiler->main_dir_path));
        dynarr_foreach(item->use.mod_path, j) {
            FirString part = dynarr_get(&item->use.mod_path, j);
            sb_printf(&use_path_b, "/%.*s", fir_string_fmt(part));
        }
        sb_print_lit(&use_path_b, ".sil");

        FirString use_path = sb_to_string(&use_path_b);
        try (add_module(compiler, use_path));
    }

    return module;
}

bool compiler_compile(Compiler *compiler, FirString main_path) {
    // strip file name
    compiler->main_dir_path = main_path;
    size_t file_name_len = 1;
    for (size_t i = main_path.len - 1; i-- > 0;) {
        file_name_len += 1;
        if (main_path.ptr[i] == '/') { break; }
    }
    compiler->main_dir_path.len -= file_name_len;

    try (add_module(compiler, main_path));

    return true;
}
