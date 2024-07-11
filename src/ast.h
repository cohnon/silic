#ifndef AST_H
#define AST_H

#include "dynarr.h"
#include "token.h"
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
    FirString span;
} AstType;

typedef enum AstExprKind {
    AstExpr_Number,
    AstExpr_String,
    AstExpr_Bool,

    AstExpr_Block,
    AstExpr_VarDef,
    AstExpr_Ret,

    AstExpr_UnOp,
    AstExpr_BinOp,

    AstExpr_ItemPath,
    AstExpr_FuncCall,
} AstExprKind;

typedef struct AstNumber {
    union {
        uint64_t integral;
        double   decimal;
    };
} AstNumber;

typedef struct AstString {
    FirString val;
} AstString;

typedef struct AstBool {
    bool val;
} AstBool;

typedef struct AstBlock {
    DynArr(AstStmt*) stmts;
} AstBlock;

typedef struct AstVar {
    FirString   name;
    AstType *type;
    AstExpr *val;
} AstVar;

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

typedef struct AstItemPath {
    DynArr(FirString) parts;
} AstItemPath;

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
        AstVar      var;
        AstRet      ret;

        AstBinOp    bin_op;
        AstUnOp     un_op;

        AstItemPath  item_path;
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

typedef enum AstItemKind {
    AstItem_Use,
    AstItem_Func,
} AstItemKind;

typedef struct AstUse {
    DynArr(FirString) mod_path;
} AstUse;

typedef struct AstFuncParam {
    FirString     name;
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

typedef struct AstItem {
    AstItemKind kind;
    bool        public;

    FirString   name;
    TextPos     pos;

    union {
        AstFunc func;
        AstUse  use;
    };
} AstItem;


typedef struct Module Module;
void ast_debug(Module *module);

bool ast_has_implicit_semicolon(AstExpr *expr);

#endif
