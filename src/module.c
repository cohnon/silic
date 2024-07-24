#include "module.h"

#include <fir/dynarr.h>
#include <stdio.h>
#include <dirent.h>


Module *module_init(String path, String source) {
    Module *module = os_alloc_T(Module);

    module->file_path = path;

    module->src = source;

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

