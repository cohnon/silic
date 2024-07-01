#include "module.h"

#include "os.h"
#include <stdio.h>


Module *module_init(FirString filepath) {
    Module *module = os_alloc_T(Module);

    module->filepath = filepath;

    OsReadFileResult read_file_result = os_read_file(filepath);
    if (!read_file_result.ok) {
        printf("couldn't find %.*s\n", fir_string_fmt(filepath));
        return NULL;
    }

    module->src = read_file_result.src;

    ns_init(&module->ns, NULL);

    dynarr_init(&module->tokens, 32);
    dynarr_init(&module->ast, 16);
    dynarr_init(&module->errors, 5);


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

