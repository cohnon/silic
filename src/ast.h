#ifndef AST_H
#define AST_H

#include "span.h"
#include "dynarr.h"
#include <fir.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct AstExpr AstExpr;
typedef struct AstStmt AstStmt;

typedef enum AstTypeKind {
    AstType_Symbol,
    AstType_Int,
} AstTypeKind;

typedef struct AstType {
    AstTypeKind kind;
    FirSym span;
} AstType;

typedef enum AstExprKind {
    AstExpr_Number,
    AstExpr_String,
    AstExpr_Bool,

    AstExpr_Block,
    AstExpr_Let,
    AstExpr_Ret,

    AstExpr_UnOp,
    AstExpr_BinOp,

    AstExpr_ModPath,
    AstExpr_FuncCall,
} AstExprKind;

typedef struct AstNumber {
    union {
        uint64_t integral;
        double   decimal;
    };
} AstNumber;

typedef struct AstString {
    FirSym val;
} AstString;

typedef struct AstBool {
    bool val;
} AstBool;

typedef struct AstBlock {
    DynArr(AstStmt*) stmts;
} AstBlock;

typedef struct AstLet {
    FirSym   name;
    AstType *type;
    AstExpr *val;
} AstLet;

typedef struct AstRet {
    AstExpr *val;
} AstRet;

typedef enum AstBinOpKind {
    AstBinOp_And,
    AstBinOp_Or,
    AstBinOp_CmpEq,
    AstBinOp_CmpNEq,
    AstBinOp_CmpGt,
    AstBinOp_CmpLt,
    AstBinOp_Assign,
    AstBinOp_Add,
    AstBinOp_Sub,
    AstBinOp_Mul,
    AstBinOp_Div,
} AstBinOpKind;

typedef struct AstBinOp {
    AstBinOpKind kind;
    AstExpr     *lhs;
    AstExpr     *rhs;
} AstBinOp;

typedef enum AstUnOpKind {
    AstUnOp_Neg,
} AstUnOpKind;

typedef struct AstUnOp {
    AstUnOpKind kind;
    AstExpr    *val;
} AstUnOp;

typedef struct AstModPath {
    DynArr(FirSym) parts;
} AstModPath;

typedef struct AstFuncCall {
    AstExpr         *target;
    DynArr(AstExpr*) args;
} AstFuncCall;

typedef struct AstExpr {
    AstExprKind kind;
    union {
        AstNumber   number;
        AstString   string;
        AstBool     bool_;

        AstBlock    block;
        AstLet      let;
        AstRet      ret;

        AstBinOp    bin_op;
        AstUnOp     un_op;

        AstModPath  mod_path;
        AstFuncCall func_call;
    };
} AstExpr;

typedef enum AstStmtKind {
    AstStmt_Expr,
} AstStmtKind;

typedef struct AstStmtExpr {
    bool     has_semicolon;
    AstExpr *val;
} AstStmtExpr;

typedef struct AstStmt {
    AstStmtKind kind;

    union {
        AstStmtExpr expr;
    };
} AstStmt;

typedef struct AstFuncParam {
    FirSym     name;
    AstType *type;
} AstFuncParam;

typedef struct AstFuncSig {
    DynArr(AstFuncParam) params;
    AstType             *ret_type;
} AstFuncSig;

typedef struct AstFunc {
    AstFuncSig sig;
    AstExpr   *body;
} AstFunc;

typedef enum AstItemKind {
    AstItem_FuncDef,
} AstItemKind;

typedef struct AstItem {
    AstItemKind kind;
    bool        public;

    FirSym      name;

    union {
        AstFunc func;
    };
} AstItem;

bool ast_has_implicit_semicolon(AstExpr *expr);

#endif
