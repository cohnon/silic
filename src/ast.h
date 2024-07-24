#ifndef AST_H
#define AST_H

#include "token.h"

#include <fir/fir.h>
#include <fir/dynarr.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct AstExpr AstExpr;

typedef enum AstTypeKind {
    AstType_Symbol,
    AstType_Int,
} AstTypeKind;

typedef struct AstType {
    AstTypeKind kind;
    String span;
} AstType;

typedef enum AstExprKind {
    // Literals
    AstExpr_Number,
    AstExpr_String,
    AstExpr_Bool,

    // statements
    AstExpr_VarDef,
    AstExpr_Ret,

    // eval
    AstExpr_Block,
    AstExpr_NamedUse,
    AstExpr_FuncCall,

    // operators
    AstExpr_UnOp,
    AstExpr_BinOp,
} AstExprKind;

typedef struct AstNumber {
    union {
        uint64_t integral;
        double   decimal;
    };
} AstNumber;

typedef struct AstString {
    String val;
} AstString;

typedef struct AstBool {
    bool val;
} AstBool;

typedef struct AstBlock {
    DynArr(AstExpr*) stmts;
    bool             is_terminated;
} AstBlock;

typedef enum AstVarDefKind {
    AstVarDef_Let,
    AstVarDef_Static,
    AstVarDef_Const,
} AstVarDefKind;

typedef struct AstVarDef {
    AstVarDefKind kind;

    String   name;
    AstType *type;
    AstExpr *val;
} AstVarDef;

typedef struct AstRet {
    AstExpr *val;
} AstRet;

typedef enum AstBinOpKind {
    // arithmetic
    AstBinOp_Add, // a + b
    AstBinOp_Sub, // a - b
    AstBinOp_Mul, // a * b
    AstBinOp_Div, // a / b

    // logic
    AstBinOp_And,    // a and b
    AstBinOp_Or,     // a or b
    AstBinOp_CmpEq,  // a == b
    AstBinOp_CmpNEq, // a != b
    AstBinOp_CmpGt,  // a > b
    AstBinOp_CmpLt,  // a < b

    // misc
    AstBinOp_Assign,         // a = b
    AstBinOp_MemberAccess,   // a.b
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

typedef struct AstNamedUse {
    // can either be 'item' or 'module:item'
    String parts[2];
    bool      qualified;
} AstNamedUse;

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
        AstVarDef   var_def;
        AstRet      ret;

        AstBinOp    bin_op;
        AstUnOp     un_op;

        AstNamedUse named_use;
        AstFuncCall func_call;
    };
} AstExpr;

typedef enum AstItemKind {
    AstItem_Use,
    AstItem_Func,
} AstItemKind;

typedef struct Module Module;
typedef struct AstUse {
    DynArr(String) mod_path;
    // if mod_path points to a directory,
    // keep track of every module in it
    DynArr(Module*)   modules;
} AstUse;

typedef struct AstFuncParam {
    String     name;
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

    String   name;
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
