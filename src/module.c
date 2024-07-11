#include "module.h"

#include "os.h"
#include <stdio.h>


Module *module_init(FirString file_path) {
    Module *module = os_alloc_T(Module);

    module->file_path = file_path;

    OsReadFileResult read_file_result = os_read_file(file_path);
    if (!read_file_result.ok) {
        printf("couldn't find %.*s\n", fir_string_fmt(file_path));
        return NULL;
    }

    module->src = read_file_result.src;

    ns_init(&module->ns, NULL);

    dynarr_init(&module->tokens, 32);
    dynarr_init(&module->ast, 16);


    return module;
}

void tok_debug(Module *module) {
    if (module->tokens.len == 0) {
        return;
    }

    printf(ANSI_BLUE "\n@@ Tokens @@\n" ANSI_RESET);

    dynarr_foreach(module->tokens, i) {
        Token *token = dynarr_get_ref(&module->tokens, i);
        printf("%s ", tok_cstr(token->kind));
    }

    printf("\n");
}

