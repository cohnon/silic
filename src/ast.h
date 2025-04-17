#ifndef AST_H
#define AST_H

#include "token.h"
#include "list.h"
#include <stdio.h>

typedef enum AstTypeKind {
    AstType_Base,
    AstType_Pointer,
    AstType_Array,
} AstTypeKind;

typedef struct AstType {
    AstTypeKind kind;
} AstType;

typedef struct AstPattern {
    Token ident;
} AstPattern;

typedef struct AstExpr AstExpr;

typedef enum AstExprKind {
    AstExpr_Block,

    AstExpr_ModuleAccess,
    AstExpr_FunctionCall,

    // literals
    AstExpr_String,
} AstExprKind;

struct AstExpr {
    AstExprKind kind;

    union {
        struct {
            AstExpr *target;
            List args;
        } call;

        Token string;
    };
};

typedef struct AstStmt AstStmt;

typedef enum AstStmtKind {
    AstStmt_Use,
    AstStmt_Let,
    AstStmt_Expr,
} AstItemKind;

typedef struct AstStmtLet {
    AstPattern pat;
    AstType *type;
    AstExpr *expr;
} AstStmtLet;

struct AstStmt {
    AstItemKind kind;

    union {
        List use;
        AstExpr *expr;
    };
};

typedef struct Ast {
    List stmts;
} Ast;

void print_ast(FILE *fp, Ast *ast);

#endif
