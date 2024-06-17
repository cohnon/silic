#include "module.h"

#include "ast.h"
#include "os.h"
#include <stdio.h>


Module *module_init(Span src) {
    Module *module = os_alloc(Module);

    module->src = src;

    array_init(&module->tokens, 32);
    array_init(&module->ast, 16);
    array_init(&module->errors, 5);

    return module;
}

void tok_debug(Module *module) {
    array_foreach(module->tokens, i) {
        Token *token = array_get_ref(&module->tokens, i);
        printf("%s ", tok_cstr(token));
    }

    printf("\n\n");
}

static void ast_debug_type(AstType *type) {
    printf("%.*s", span_fmt(type->span));
}

static void ast_debug_item(AstItem *item) {
    printf("@@ Item @@\n");
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
    printf("\n\n");
}

void ast_debug(Module *module) {
    array_foreach(module->ast, i) {
        ast_debug_item(array_get(&module->ast, i));
    }
}
