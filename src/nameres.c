#include "nameres.h"

#include "try.h"
#include <fir/map.h>


typedef struct NameresCtx {
    Compiler *compiler;
    Namespace global_ns;
    Namespace *cur_ns;
} NameresCtx;


// namespace
static void ns_init(Namespace *ns, Namespace *parent) {
    ns->parent = parent;
    map_init(&ns->symbols, 8);
}

static void ns_deinit(Namespace *ns) {
    map_deinit(&ns->symbols);
}

static AstDef *ns_get(Namespace *ns, String name) {
    Namespace *cur = ns;
    while (cur != NULL) {
        AstDef *entry = map_get_ref(&ns->symbols, name);

        if (entry != NULL) { return entry; }

        cur = cur->parent;
    }

    return NULL;
}

static AstDef *ns_insert_func(Namespace *ns, AstItem *item) {
    AstDef entry = {
        AstDef_Func,
        { .item=item },
    };

    return map_insert(&ns->symbols, item->name, &entry);
}

/*
static void enter_scope(NameresCtx *ctx, Namespace *ns) {
    ns_init(ns, ctx->cur_ns);
    ctx->cur_ns = ns;
}

static void exit_scope(NameresCtx *ctx) {
    Namespace *ns = ctx->cur_ns;
    ctx->cur_ns = ns->parent;
    ns_deinit(ns);
}
*/

static bool scan_module(NameresCtx *ctx, Module *module) {
    dynarr_foreach(module->ast, i) {
        AstItem *item = dynarr_get(&module->ast, i);
       
        // uses aren't scanned
        if (item->kind == AstItem_Use) {
            continue;
        }

        AstDef *existing = ns_insert_func(ctx->cur_ns, item);
        if (existing != NULL) {
            error_add(
                ctx->compiler,
                ErrorMsg_NameConflict,
                module->file_path,
                item->name,
                item->pos
            );
            return false;
        }
    }

    return true;
}

bool resolve_names(Compiler *compiler) {
    NameresCtx ctx;
    ctx.compiler = compiler;
    ns_init(&ctx.global_ns, NULL);
    ctx.cur_ns = &ctx.global_ns;

    dynarr_foreach(compiler->modules, i) {
        Module *module = dynarr_get(&compiler->modules, i);
        try (scan_module(&ctx, module));
    }

    dynarr_foreach(compiler->modules, i) {
        //Module *module = dynarr_get(&compiler->modules, i);
        //try (resolve_module(&ctx, module));
    }

    return true;
}
