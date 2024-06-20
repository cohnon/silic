#ifndef AST_H
#define AST_H

#include "span.h"
#include "array.h"
#include <stdbool.h>
#include <stdint.h>


typedef enum AstTypeKind {
    AstType_Symbol,
    AstType_Int,
} AstTypeKind;

typedef struct AstType {
    AstTypeKind kind;
    Span span;
} AstType;

typedef enum AstExprKind {
    AstExpr_Number,
    AstExpr_String,
    AstExpr_Bool,

    AstExpr_Let,
    AstExpr_Ret,

    AstExpr_UnOp,
    AstExpr_BinOp,

    AstExpr_ModPath,
    AstExpr_FuncCall,
} AstExprKind;

typedef struct AstExpr AstExpr;

typedef struct AstNumber {
    union {
        int64_t integral;
        double  decimal;
    };
} AstNumber;

typedef struct AstString {
    Span val;
} AstString;

typedef struct AstBool {
    bool val;
} AstBool;

typedef struct AstLet {
    Span     name;
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
    Array(Span) parts;
} AstModPath;

typedef struct AstFuncCall {
    AstExpr        *target;
    Array(AstExpr*) args;
} AstFuncCall;

typedef struct AstExpr {
    AstExprKind kind;
    union {
        AstNumber   num;
        AstString   str;
        AstBool     bool_;

        AstLet      let;
        AstRet      ret;

        AstBinOp    bin_op;
        AstUnOp     un_op;

        AstModPath  mod_path;
        AstFuncCall func_call;
    };
} AstExpr;

typedef struct AstFuncParam {
    Span     name;
    AstType *type;
} AstFuncParam;

typedef struct AstFuncSig {
    Array(AstFuncParam) params;
    AstType            *ret_type;
} AstFuncSig;

typedef struct AstFunc {
    AstFuncSig sig;
} AstFunc;

typedef enum AstItemKind {
    AstItem_FuncDef,
} AstItemKind;

typedef struct AstItem {
    AstItemKind kind;
    bool public;

    Span name;

    union {
        AstFunc func;
    };
} AstItem;

#endif
