#include "ast.h"

#include "module.h"
#include <inttypes.h>
#include <stdio.h>


bool ast_has_implicit_semicolon(AstExpr *expr) {
    AstExprKind kind = expr->kind;

    return (kind == AstExpr_Block);
}

static void ast_debug_type(AstType *type) {
    printf("%.*s", string_fmt(type->span));
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
    case AstExpr_VarDef: {
        printf("VAR_DEF(%.*s)\n", string_fmt(expr->var_def.name));
        ast_debug_expr(expr->var_def.val, indent_lvl + 1);
        break;
    }
    case AstExpr_NamedUse: {
        printf("NAMED_USE(");
        String *parts = expr->named_use.parts;
        printf("%.*s", string_fmt(parts[0]));

        if (expr->named_use.qualified) {
            printf(":%.*s", string_fmt(parts[1]));
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
        break;
    }
    case AstExpr_Block: {
        printf("BLOCK\n");
        dynarr_foreach(expr->block.stmts, i) {
            AstExpr *block = dynarr_get(&expr->block.stmts, i);
            ast_debug_expr(block, indent_lvl + 1);
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
    dynarr_foreach(item->use.mod_path, i) {
        String sub_mod = dynarr_get(&item->use.mod_path, i);

        printf("%.*s", string_fmt(sub_mod));

        if (i + 1 < item->use.mod_path.len) {
            printf("/");
        }
    }

    printf("\n");
}

static void ast_debug_func(AstItem *item) {
    printf("func %.*s(", string_fmt(item->name));

    dynarr_foreach(item->func.sig.params, i) {
        AstFuncParam *param = dynarr_get_ref(&item->func.sig.params, i);
        printf("%.*s: ", string_fmt(param->name));
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
    switch (item->kind) {
    case AstItem_Use: break; // we printed all uses before other items
    case AstItem_Func: ast_debug_func(item); break;
    }
}

void ast_debug(Module *module) {
    printf(ANSI_BLUE "\n@@ File @@\n" ANSI_RESET "%.*s\n", string_fmt(module->file_path));

    // only print 'Imports' title if module has imports
    bool has_imports = false;
    if (module->ast.len > 0) {
        dynarr_foreach(module->ast, i) {
            AstItem *item = dynarr_get(&module->ast, i);
            if (item->kind != AstItem_Use) { continue; }
            if (!has_imports) {
                printf(ANSI_BLUE "Imports\n" ANSI_RESET);
                has_imports = true;
            }
            ast_debug_use(item);
        }
    }

    printf(ANSI_BLUE "Items\n" ANSI_RESET);
    dynarr_foreach(module->ast, i) {
        ast_debug_item(dynarr_get(&module->ast, i));
    }
}

