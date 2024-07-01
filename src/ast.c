#include "ast.h"

#include "os.h"
#include "module.h"
#include <inttypes.h>
#include <stdio.h>


bool ast_has_implicit_semicolon(AstExpr *expr) {
    AstExprKind kind = expr->kind;

    return (kind == AstExpr_Block);
}

static void ast_debug_type(AstType *type) {
    printf("%.*s", fir_string_fmt(type->span));
}

static void ast_debug_expr(AstExpr *expr, int indent_lvl) {
    printf(ANSI_GREY);
    for (int i = 0; i < indent_lvl - 1; i += 1) {
        printf("│ ");
    }
    printf("├─");
    printf(ANSI_RESET);

    switch (expr->kind) {
    case AstExpr_Number: {
        printf("NUMBER(%" PRIu64 ")\n", expr->number.integral);
        break;
    }
    case AstExpr_ItemPath: {
        printf("ITEM_PATH(");
        dynarr_foreach(expr->item_path.parts, i) {
            FirString part = dynarr_get(&expr->item_path.parts, i);
            printf("%.*s", fir_string_fmt(part));

            if (i + 1 < expr->item_path.parts.len) {
                printf(":");
            }
        }
        printf(")\n");
        break;
    }
    case AstExpr_FuncCall: {
        printf("FUNC_CALL\n");
        ast_debug_expr(expr->func_call.target, indent_lvl + 1);
        dynarr_foreach(expr->func_call.args, i) {
            AstExpr *arg = dynarr_get(&expr->func_call.args, i);
            ast_debug_expr(arg, indent_lvl + 1);
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

static void ast_debug_use(AstItem *item) {
    printf("use ");

    dynarr_foreach(item->use.mod_path, i) {
        FirString sub_mod = dynarr_get(&item->use.mod_path, i);

        printf("%.*s", fir_string_fmt(sub_mod));

        if (i + 1 < item->use.mod_path.len) {
            printf("/");
        }
    }

    printf(";\n");
}

static void ast_debug_func(AstItem *item) {
    printf("func %.*s(", fir_string_fmt(item->name));

    dynarr_foreach(item->func.sig.params, i) {
        AstFuncParam *param = dynarr_get_ref(&item->func.sig.params, i);
        printf("%.*s: ", fir_string_fmt(param->name));
        ast_debug_type(param->type);
        if (i < item->func.sig.params.len - 1) {
            printf(", ");
        }
    }

    printf(") -> ");
    
    if (item->func.sig.ret_type == NULL) {
        printf("Void");
    } else {
        ast_debug_type(item->func.sig.ret_type);
    }

    printf("\n");

    ast_debug_expr(item->func.body, 1);
}

static void ast_debug_item(AstItem *item) {
    printf(ANSI_BLUE "\nItem\n" ANSI_RESET);

    switch (item->kind) {
    case AstItem_Use: __builtin_unreachable();
    case AstItem_Func: ast_debug_func(item); break;
    }
}

void ast_debug(Module *module) {
    printf(ANSI_BLUE "\n@@ File @@\n" ANSI_RESET "%.*s\n", fir_string_fmt(module->filepath));

    if (module->uses.len > 0) {
        printf(ANSI_BLUE "\nImports\n" ANSI_RESET);
        dynarr_foreach(module->uses, i) {
            ast_debug_use(dynarr_get(&module->uses, i));
        }
    }

    dynarr_foreach(module->ast, i) {
        ast_debug_item(dynarr_get(&module->ast, i));
    }
}

