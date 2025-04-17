#include "ast.h"

static void print_expr(FILE *fp, AstExpr *expr) {
    fprintf(fp, "(");
    switch (expr->kind) {
        case AstExpr_String: fprintf(fp, "string lit"); break;
        case AstExpr_Block: fprintf(fp, "block"); break;
        case AstExpr_ModuleAccess: fprintf(fp, "module access"); break;
        case AstExpr_FunctionCall: fprintf(fp, "function call"); break;
    }
    fprintf(fp, ")");
}

static void print_stmt(FILE *fp, AstStmt *stmt) {
    switch (stmt->kind) {
        case AstStmt_Use: fprintf(fp, "(use)\n"); break;
        case AstStmt_Let:
            fprintf(fp, "(let ");
            print_expr(fp, stmt->expr);
            fprintf(fp, ")\n");
            break;
        case AstStmt_Expr:
            print_expr(fp, stmt->expr);
            break;
        default: fprintf(fp, "(unhandled stmt)\n"); break;
    }
}

void print_ast(FILE *fp, Ast *ast) {
    for (unsigned int i = 0; i < ast->stmts.len; i += 1) {
        AstStmt *stmt = list_get(&ast->stmts, AstStmt *, i);
        print_stmt(fp, stmt);
    }
}

