#include "module.h"

#include "ast.h"
#include "os.h"
#include <stdio.h>
#include <inttypes.h>


Module *module_init(FirSym filepath) {
    Module *module = os_alloc_T(Module);

    module->filepath = filepath;

    OsReadFileResult read_file_result = os_read_file(filepath);
    if (!read_file_result.ok) {
        printf("couldn't find %.*s\n", fir_sym_fmt(filepath));
        return NULL;
    }

    module->src = read_file_result.src;

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

static void ast_debug_type(AstType *type) {
    printf("%.*s", fir_sym_fmt(type->span));
}

static void ast_debug_expr(AstExpr *expr, int indent_lvl) {
    printf(ANSI_GREY);
    for (int i = 0; i < indent_lvl; i += 1) {
        printf("│ ");
    }
    printf(ANSI_RESET);

    switch (expr->kind) {
    case AstExpr_Number: {
        printf("NUMBER(%" PRIu64 ")\n", expr->number.integral);
        break;
    }
    case AstExpr_ModPath: {
        printf("MOD_PATH(");
        dynarr_foreach(expr->mod_path.parts, i) {
            FirSym part = dynarr_get(&expr->mod_path.parts, i);
            printf("%.*s", fir_sym_fmt(part));

            if (i + 1 < expr->mod_path.parts.len) {
                printf(":");
            }
        }
        printf(")\n");
        break;
    }
    case AstExpr_Block: {
        printf("BLOCK\n");
        dynarr_foreach(expr->block.stmts, i) {
            AstStmt *stmt = dynarr_get(&expr->block.stmts, i);
            ast_debug_expr(stmt->expr.val, indent_lvl + 1);
        }
        break;
    }
    case AstExpr_BinOp: {
        printf("BINOP(");
        switch (expr->bin_op.kind) {
        case AstBinOp_Add: printf("+"); break;
        case AstBinOp_Sub: printf("-"); break;
        case AstBinOp_Mul: printf("*"); break;
        case AstBinOp_Div: printf("/"); break;
        default: printf("<unhandled operator>");
        }
        printf(")\n");
        ast_debug_expr(expr->bin_op.lhs, indent_lvl + 1);
        ast_debug_expr(expr->bin_op.rhs, indent_lvl + 1);
        break;
    }
    default: printf("<unhandled expr>\n"); break;
    }
}

static void ast_debug_item(AstItem *item) {
    printf(ANSI_BLUE "\n@@ Item @@\n" ANSI_RESET);
    printf("func %.*s(", fir_sym_fmt(item->name));

    dynarr_foreach(item->func.sig.params, i) {
        AstFuncParam *param = dynarr_get_ref(&item->func.sig.params, i);
        printf("%.*s: ", fir_sym_fmt(param->name));
        ast_debug_type(param->type);
        if (i < item->func.sig.params.len - 1) {
            printf(", ");
        }
    }

    printf(") -> ");
    ast_debug_type(item->func.sig.ret_type);
    printf("\n");

    ast_debug_expr(item->func.body, 0);
}

void ast_debug(Module *module) {
    dynarr_foreach(module->ast, i) {
        ast_debug_item(dynarr_get(&module->ast, i));
    }
}
