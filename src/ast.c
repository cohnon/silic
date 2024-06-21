#include "ast.h"


bool ast_has_implicit_semicolon(AstExpr *expr) {
    AstExprKind kind = expr->kind;

    return (kind == AstExpr_Block);
}

