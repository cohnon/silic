#include "nameres.h"

#include "try.h"


typedef struct NameresCtx {
    Compiler *compiler;
    Namespace global_ns;
    Namespace *cur_ns;
} NameresCtx;

static void enter_scope(NameresCtx *ctx, Namespace *ns) {
    ns_init(ns, ctx->cur_ns);
    ctx->cur_ns = ns;
}

static void exit_scope(NameresCtx *ctx) {
    ctx->cur_ns = ctx->cur_ns->parent;
}

static bool resolve_use(NameresCtx *ctx, AstItem *item) {
    return true;
}

static bool resolve_func(NameresCtx *ctx, AstItem *item) {
    return true;
}

static bool resolve_item(NameresCtx *ctx, AstItem *item) {
    switch (item->kind) {
    case AstItem_Use: return resolve_use(ctx, item);
    case AstItem_Func: return resolve_func(ctx, item);
    default: return true;
    }
}

static bool resolve_module(NameresCtx *ctx, Module *module) {
    dynarr_foreach(module->ast, i) {
        AstItem *item = dynarr_get(&module->ast, i);
        try (resolve_item(ctx, item));
    }

    return true;
}

static bool scan_module(NameresCtx *ctx, Module *module) {
    dynarr_foreach(module->ast, i) {
        AstItem *item = dynarr_get(&module->ast, i);
       
        // uses aren't scanned
        if (item->kind == AstItem_Use) {
            continue;
        }

        NsEntry *existing = ns_add_item(ctx->compiler, item);
        if (true) {
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
        Module *module = dynarr_get(&compiler->modules, i);
        try (resolve_module(&ctx, module));
    }

    return true;
}
