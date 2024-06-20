#include "module.h"

#include "ast.h"
#include "os.h"
#include <stdio.h>


Module *module_init(Span filepath) {
    Module *module = os_alloc_T(Module);

    module->filepath = filepath;

    OsReadFileResult read_file_result = os_read_file(filepath);
    if (!read_file_result.ok) {
        printf("couldn't find %.*s\n", span_fmt(filepath));
        return NULL;
    }

    module->src = read_file_result.src;

    array_init(&module->tokens, 32);
    array_init(&module->ast, 16);
    array_init(&module->errors, 5);

    return module;
}

void tok_debug(Module *module) {
    if (module->tokens.len == 0) {
        return;
    }

    array_foreach(module->tokens, i) {
        Token *token = array_get_ref(&module->tokens, i);
        printf("%s ", tok_cstr(token->kind));
    }

    printf("\n");
}

static void ast_debug_type(AstType *type) {
    printf("%.*s", span_fmt(type->span));
}

static void ast_debug_item(AstItem *item) {
    printf("\n@@ Item @@\n");
    printf("func %.*s(", span_fmt(item->name));

    array_foreach(item->func.sig.params, i) {
        AstFuncParam *param = array_get_ref(&item->func.sig.params, i);
        printf("%.*s: ", span_fmt(param->name));
        ast_debug_type(param->type);
        if (i < item->func.sig.params.len - 1) {
            printf(", ");
        }
    }

    printf(") -> ");
    ast_debug_type(item->func.sig.ret_type);
    printf("\n");
}

void ast_debug(Module *module) {
    array_foreach(module->ast, i) {
        ast_debug_item(array_get(&module->ast, i));
    }
}
