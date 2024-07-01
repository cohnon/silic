#include "parser.h"

#include "ast.h"
#include "os.h"
#include "try.h"
#include <assert.h>


typedef struct ParserCtx {
    Module *module;
    size_t tok_idx;
} ParserCtx;

static Token *cur_tok(ParserCtx *ctx) {
    return dynarr_get_ref(&ctx->module->tokens, ctx->tok_idx);
}

static bool cur_tok_is(ParserCtx *ctx, TokenKind kind) {
    return cur_tok(ctx)->kind == kind;
}

static Token *eat_tok(ParserCtx *ctx) {
    Token *tok = cur_tok(ctx);

    ctx->tok_idx += 1;

    return tok;
}

static bool eat_tok_if(ParserCtx *ctx, TokenKind kind) {
    if (cur_tok_is(ctx, kind)) {
        ctx->tok_idx += 1;
        return true;
    }

    return false;
}

static Token *expect_tok(ParserCtx *ctx, TokenKind kind) {
    if (!cur_tok_is(ctx, kind)) {
        // create a token for the expected token
        Token *expected_tok = os_alloc_T(Token);

        if (ctx->tok_idx > 0) {
            
            Token *prev_tok = &ctx->module->tokens.ptr[ctx->tok_idx - 1];

            expected_tok->pos = prev_tok->pos;
            expected_tok->pos.col += prev_tok->span.len;

            expected_tok->span = prev_tok->span;
            expected_tok->span.ptr += prev_tok->span.len;
            expected_tok->span.len = 1;

        } else {

            // an error on the first token is a special case
            expected_tok->pos = (TextPos){ 1, 1 };
            expected_tok->span = cur_tok(ctx)->span;

        }

        ErrorMsgId error = error_add(ctx->module, expected_tok, "syntax error");
        error_hint(ctx->module, error, "expected %s here", tok_cstr(kind));

        return NULL;
    }

    return eat_tok(ctx);
}

static AstExpr *parse_expr(ParserCtx *ctx);
static AstStmt *parse_stmt(ParserCtx *ctx);

static AstType *parse_type(ParserCtx *ctx) {
    AstType *type = os_alloc_T(AstType);

    Token *type_tok = try (expect_tok(ctx, Token_Symbol));
    type->span = type_tok->span;

    return type;
}

static AstExpr *parse_number(ParserCtx *ctx) {
    AstExpr *expr = os_alloc_T(AstExpr);
    expr->kind = AstExpr_Number;

    Token *num_tok = try (expect_tok(ctx, Token_Number));

    // TODO: handle decimal
    expr->number.integral = 0;

    for (size_t i = 0; i < num_tok->span.len; i += 1) {
        char digit_char = num_tok->span.ptr[i];
        uint64_t digit = digit_char - '0';

        expr->number.integral *= 10;
        expr->number.integral += digit;
    }

    return expr;
}

static AstExpr *parse_item_path(ParserCtx *ctx) {
    AstExpr *expr = os_alloc_T(AstExpr);
    expr->kind = AstExpr_ItemPath;
    dynarr_init(&expr->item_path.parts, 2);

    while (true) {
        Token *sym_tok = try (expect_tok(ctx, Token_Symbol));
        dynarr_push(&expr->item_path.parts, (&(sym_tok->span)));
        
        if (!eat_tok_if(ctx, Token_Colon)) {
            break;
        }
    }

    return expr;
}

static AstExpr *parse_block(ParserCtx *ctx) {
    AstExpr *expr = os_alloc_T(AstExpr);
    expr->kind = AstExpr_Block;
    dynarr_init(&expr->block.stmts, 8);

    expect_tok(ctx, Token_LBrace);

    while (!cur_tok_is(ctx, Token_RBrace)) {
        AstStmt *stmt = try (parse_stmt(ctx));
        dynarr_push(&expr->block.stmts, &stmt);

        // only the last statement of a block will have no semicolon
        // (unless it's a kind of control flow)
        if (stmt->kind == AstStmt_Expr
        && !stmt->expr.has_semicolon
        && !ast_has_implicit_semicolon(stmt->expr.val)) {
            break;
        }
    }

    // if we finish the statements and don't reach a '}'
    // there is a missing semicolon
    if (!eat_tok_if(ctx, Token_RBrace)) {
        assert(!cur_tok_is(ctx, Token_Semicolon));

        try (expect_tok(ctx, Token_Semicolon));
    }

    return expr;
}

static AstExpr *parse_grouped_expr(ParserCtx *ctx) {
    try (expect_tok(ctx, Token_LParen));

    AstExpr *expr = try (parse_expr(ctx));
    try (expect_tok(ctx, Token_RParen));

    return expr;
}

static AstExpr *parse_func_call(ParserCtx *ctx, AstExpr *target) {
    AstExpr *expr = os_alloc_T(AstExpr);
    expr->kind = AstExpr_FuncCall;

    expr->func_call.target = target;
    dynarr_init(&expr->func_call.args, 4);

    try (expect_tok(ctx, Token_LParen));

    while (!cur_tok_is(ctx, Token_RParen)) {
        AstExpr *arg = try (parse_number(ctx));
        dynarr_push(&expr->func_call.args, &arg);

        if (!eat_tok_if(ctx, Token_Comma)) {
            break;
        }
    }

    try (expect_tok(ctx, Token_RParen));

    return expr;
}

static AstExpr *parse_suffix(ParserCtx *ctx, AstExpr *target) {
    while (true) {
        switch (cur_tok(ctx)->kind) {
        case Token_LParen: target = parse_func_call(ctx, target); break;
        default: return target;
        }
    }
}

static AstExpr *parse_expr_primary(ParserCtx *ctx) {
    AstExpr *expr = NULL;

    switch (cur_tok(ctx)->kind) {
    case Token_Number: expr = try (parse_number(ctx)); break;
    case Token_Symbol: expr = try (parse_item_path(ctx)); break;
    case Token_LBrace: expr = try (parse_block(ctx)); break;
    case Token_LParen: expr = try (parse_grouped_expr(ctx)); break;
    default: {
        ErrorMsgId err = error_add(ctx->module, cur_tok(ctx), "syntax error");
        error_hint(ctx->module, err, "expected expression");
        break;
    }
    }

    return parse_suffix(ctx, expr);
}

typedef struct Operator {
    AstBinOpKind kind;
    int          prec_l;
    int          prec_r;
} Operator;

static Operator parse_operator(ParserCtx *ctx) {
    switch (cur_tok(ctx)->kind) {
    case Token_Plus: return (Operator){ AstBinOp_Add, 1, 2 };
    case Token_Dash: return (Operator){ AstBinOp_Sub, 1, 2 };

    case  Token_Star: return (Operator){ AstBinOp_Mul, 3, 4 };
    case Token_Slash: return (Operator){ AstBinOp_Div, 3, 4 };

    default: return (Operator){ 0, -1, -1 };
    }
}

static AstExpr *parse_expr_prec(ParserCtx *ctx, int prec) {
    AstExpr *lhs = try (parse_expr_primary(ctx));

    while (true) {
        Operator op = parse_operator(ctx);

        // no more operators
        if (op.prec_l == -1) { break; }

        if (op.prec_l < prec) { break; }

        // consume the operator
        eat_tok(ctx);

        AstExpr *rhs = try (parse_expr_prec(ctx, op.prec_r));

        AstExpr *op_expr = os_alloc_T(AstExpr);
        op_expr->kind = AstExpr_BinOp;

        op_expr->bin_op.kind = op.kind;
        op_expr->bin_op.lhs = lhs;
        op_expr->bin_op.rhs = rhs;

        lhs = op_expr;
    }

    return lhs;
}

static AstExpr *parse_expr(ParserCtx *ctx) {
    return parse_expr_prec(ctx, 0);
}

static AstStmt *parse_stmt(ParserCtx *ctx) {
    AstStmt *stmt = os_alloc_T(AstStmt);

    switch (cur_tok(ctx)->kind) {
    default: {
        stmt->kind = AstStmt_Expr;
        stmt->expr.val = try (parse_expr(ctx));
        stmt->expr.has_semicolon = eat_tok_if(ctx, Token_Semicolon);
        break;
    }
    }

    return stmt;
}

static AstItem *parse_use(ParserCtx *ctx, AstItem *item) {
    item->kind = AstItem_Use;
    dynarr_init(&item->use.mod_path, 4);

    try (expect_tok(ctx, Token_Use));

    while (true) {
        Token *submod_tok = try (expect_tok(ctx, Token_Symbol));

        dynarr_push(&item->use.mod_path, &submod_tok->span);

        if (!eat_tok_if(ctx, Token_Slash)) {
            break;
        }
    }

    // unqualified items
    if (eat_tok_if(ctx, Token_Colon)) {
        try (expect_tok(ctx, Token_LBrace));

        while (!cur_tok_is(ctx, Token_RBrace)) {
            try (expect_tok(ctx, Token_Symbol));

            if (!eat_tok_if(ctx, Token_Comma)) {
                break;
            }
        }

        try (expect_tok(ctx, Token_RBrace));
    }

    return item;
}

static AstItem *parse_func(ParserCtx *ctx, AstItem *item) {
    item->kind = AstItem_Func;
    dynarr_init(&item->func.sig.params, 8);

    try (expect_tok(ctx, Token_Func));

    Token *func_name_tok = try (expect_tok(ctx, Token_Symbol));
    item->name = func_name_tok->span;

    try (expect_tok(ctx, Token_LParen));

    while (!cur_tok_is(ctx, Token_RParen)) {
        Token *param_name_tok = try (expect_tok(ctx, Token_Symbol));
        try (expect_tok(ctx, Token_Colon));
        AstType *param_type = try (parse_type(ctx));

        AstFuncParam param = (AstFuncParam){
            .name = param_name_tok->span,
            .type = param_type
        };

        dynarr_push(&item->func.sig.params, &param);

        if (!eat_tok_if(ctx, Token_Comma)) {
            break;
        }
    }

    try (expect_tok(ctx, Token_RParen));

    if (eat_tok_if(ctx, Token_RArrow)) {
        item->func.sig.ret_type = try (parse_type(ctx));
    }


    item->func.body = try (parse_block(ctx));

    return item;
}

static AstItem *parse_item(ParserCtx *ctx) {
    AstItem *item = os_alloc_T(AstItem);

    item->public = false;
    if (eat_tok_if(ctx, Token_Pub)) {
        item->public = true;
    }

    switch (cur_tok(ctx)->kind) {
    case Token_Use: return parse_use(ctx, item);
    case Token_Func: return parse_func(ctx, item);
    default: {
        error_add(ctx->module, cur_tok(ctx), "syntax error");
        return NULL;
    };
    }
}

bool parse_module(Module *module) {
    ParserCtx ctx;
    ctx.module = module;
    ctx.tok_idx = 0;

    while (!cur_tok_is(&ctx, Token_Eof)) {
        AstItem *item = try (parse_item(&ctx));

        if (item->kind == AstItem_Use) {
            dynarr_push(&ctx.module->uses, &item);
        } else {
            dynarr_push(&ctx.module->ast, &item);
        }
    }

    return true;
}
