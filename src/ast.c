#include "ast.h"

static void print_expression(FILE *fp, AstExpr *expr);

static void print_indent(FILE *fp, int indent) {
    for (int i = 0; i < indent; i += 1) {
        fprintf(fp, "  ");
    }
}

static void print_binary_operator(FILE *fp, AstExpr *expr) {
    switch (expr->bin_op.kind) {
        case AstExprBinOp_Add: fprintf(fp, "(+ "); break;
        case AstExprBinOp_Sub: fprintf(fp, "(- "); break;
        case AstExprBinOp_Mul: fprintf(fp, "(* "); break;
        case AstExprBinOp_Div: fprintf(fp, "(/ "); break;
    }

    print_expression(fp, expr->bin_op.lhs);
    fprintf(fp, " ");
    print_expression(fp, expr->bin_op.rhs);

    fprintf(fp, ")");
}

static void print_block(FILE *fp, AstExpr *expr, int indent) {
    fprintf(fp, "(\n");
    for (size_t i = 0; i < expr->block.exprs.len; i += 1) {
        print_indent(fp, indent);
        print_expression(fp, list_get(&expr->block.exprs, AstExpr *, i));
        fprintf(fp, "\n");
    }
    print_indent(fp, indent - 1);
    fprintf(fp, ")");
}

static void print_ident(FILE *fp, AstExpr *expr) {
    fprintf(fp, "%.*s", expr->ident.tok.len, expr->ident.tok.src);
}

static void print_member_access(FILE *fp, AstExpr *expr) {
    print_expression(fp, expr->mem_access.target);
    fprintf(fp, ".");
    print_expression(fp, expr->mem_access.member);
}

static void print_function_call(FILE *fp, AstExpr *expr) {
    fprintf(fp, "(call ");
    print_expression(fp, expr->fn_call.target);

    for (size_t i = 0; i < expr->fn_call.args.len; i += 1) {
        fprintf(fp, " ");
        AstExpr *arg = list_get(&expr->fn_call.args, AstExpr *, i);
        print_expression(fp, arg);
    }

    fprintf(fp, ")");
}

static void print_expression(FILE *fp, AstExpr *expr) {
    switch (expr->kind) {
        case AstExpr_BinOp: print_binary_operator(fp, expr); break;
        case AstExpr_Block: print_block(fp, expr, 1); break;
        case AstExpr_Identifier: print_ident(fp, expr); break;
        case AstExpr_MemberAccess: print_member_access(fp, expr); break;
        case AstExpr_FunctionCall: print_function_call(fp, expr); break;
        case AstExpr_String: fprintf(fp, "(string lit)"); break;
    }
}

static void print_statement(FILE *fp, AstStmt *stmt) {
    switch (stmt->kind) {
        case AstStmt_Use: fprintf(fp, "(use)\n"); break;
        case AstStmt_Let:
            fprintf(fp, "(let ");
            print_expression(fp, stmt->expr);
            fprintf(fp, ")\n");
            break;
        case AstStmt_Expr:
            print_expression(fp, stmt->expr);
            break;
        default: fprintf(fp, "(unhandled stmt)\n"); break;
    }
}

void print_ast(FILE *fp, Ast *ast) {
    for (unsigned int i = 0; i < ast->stmts.len; i += 1) {
        AstStmt *stmt = list_get(&ast->stmts, AstStmt *, i);
        print_statement(fp, stmt);
    }
}

