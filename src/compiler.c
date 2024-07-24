#include "compiler.h"

#include "lexer.h"
#include "nameres.h"
#include "parser.h"
#include "try.h"

#include <fir/os.h>
#include <fir/string_builder.h>
#include <fir/string.h>
#include <stdio.h>
#include <dirent.h>


Compiler *compiler_init(void) {
    Compiler *compiler = os_alloc_T(Compiler);

    dynarr_init(&compiler->modules, 4);
    dynarr_init(&compiler->errors, 4);

    return compiler;
}

// mod_path should be a full path to a .sil file
static Module *add_module(Compiler *compiler, String mod_path) {
    OsReadFileResult read_file_result = os_read_file(mod_path);
    if (!read_file_result.ok) {
        printf("couldn't find %.*s\n", string_fmt(mod_path));
        return NULL;
    }

    Module *module = try (module_init(mod_path, read_file_result.src));

    try_else (lex_module(compiler, module), error_print(compiler));

    // tok_debug(module);

    try_else (parse_module(compiler, module), error_print(compiler));

    ast_debug(module);

    dynarr_push(&compiler->modules, &module);

    dynarr_foreach(module->ast, i) {
        AstItem *item = dynarr_get(&module->ast, i);
        if (item->kind != AstItem_Use) { break; }

        // create the path to module
        String use_path = string_clone(compiler->main_dir_path);
        dynarr_foreach(item->use.mod_path, j) {
            String part = dynarr_get(&item->use.mod_path, j);
            filepath_extend(&use_path, part);
        }

        if (os_is_dir(use_path)) {
            // import entire contents of directory
            dynarr_init(&item->use.modules, 8);

            Directory dir = os_open_dir(use_path);
            while (true) {
                char *file_path = os_next_file_in_dir(dir);
                if (file_path == NULL) { break; }

                Module *new_module = try (add_module(
                    compiler,
                    filepath_join(
                        use_path,
                        string_cstr(file_path)
                    )
                ));

                dynarr_push(&item->use.modules, &new_module);
            }
            os_close_dir(dir);

        } else {
            // import single file module
            string_append(&use_path, string_lit(".sil"));
            try (add_module(compiler, use_path));
        }
    }

    return module;
}

bool compiler_compile(Compiler *compiler, String main_path) {
    // strip file name
    compiler->main_dir_path = main_path;
    size_t file_name_len = 1;
    for (size_t i = main_path.len - 1; i-- > 0;) {
        file_name_len += 1;
        if (main_path.ptr[i] == '/') { break; }
    }
    compiler->main_dir_path.len -= file_name_len;

    try (add_module(compiler, main_path));

    try_else (resolve_names(compiler), error_print(compiler));

    return true;
}
