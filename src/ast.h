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
    AstExpr_BinOp,
    AstExpr_Block,

    AstExpr_Identifier,
    AstExpr_MemberAccess,
    AstExpr_FunctionCall,

    // literals
    AstExpr_String,
} AstExprKind;

typedef enum AstExprBinOpKind {
    AstExprBinOp_Add,
    AstExprBinOp_Sub,
    AstExprBinOp_Mul,
    AstExprBinOp_Div,
} AstExprBinOpKind;

typedef struct AstExprBinOp {
    AstExprBinOpKind kind;
    AstExpr *lhs, *rhs;
} AstExprBinOp;

typedef struct AstExprBlock {
    List exprs;
} AstExprBlock;

typedef struct AstExprIdentifier {
    Token tok;
} AstExprIdentifier;

typedef struct AstExprMemberAccess {
    AstExpr *target;
    AstExpr *member;
} AstExprMemberAccess;

typedef struct AstExprFunctionCall {
    AstExpr *target;
    List args;
} AstExprFunctionCall;

typedef struct AstExprString {
    Token tok;
} AstExprString;

struct AstExpr {
    AstExprKind kind;

    union {
        AstExprBinOp bin_op;
        AstExprBlock block;
        AstExprIdentifier ident;
        AstExprMemberAccess mem_access;
        AstExprFunctionCall fn_call;
        AstExprString string;
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
