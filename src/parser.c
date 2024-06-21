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

        Token *prev_tok = &ctx->module->tokens.ptr[ctx->tok_idx - 1];

        expected_tok->pos = prev_tok->pos;
        expected_tok->pos.col += prev_tok->span.len;

        expected_tok->span = prev_tok->span;
        expected_tok->span.ptr += prev_tok->span.len;
        expected_tok->span.len = 1;

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

static AstExpr *parse_mod_path(ParserCtx *ctx) {
    AstExpr *expr = os_alloc_T(AstExpr);
    expr->kind = AstExpr_ModPath;
    dynarr_init(&expr->mod_path.parts, 2);

    while (true) {
        Token *sym_tok = try (expect_tok(ctx, Token_Symbol));
        dynarr_push(&expr->mod_path.parts, (&(sym_tok->span)));
        
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

static AstExpr *parse_expr_primary(ParserCtx *ctx) {
    switch (cur_tok(ctx)->kind) {
    case Token_Number: return parse_number(ctx);
    case Token_Symbol: return parse_mod_path(ctx);
    case Token_LBrace: return parse_block(ctx);
    case Token_LParen: {
        eat_tok(ctx);
        AstExpr *expr = try (parse_expr(ctx));
        try (expect_tok(ctx, Token_RParen));

        return expr;
    }
    default: return NULL;
    }
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

static AstItem *parse_item(ParserCtx *ctx) {
    AstItem *item = os_alloc_T(AstItem);

    item->public = false;
    if (eat_tok_if(ctx, Token_Pub)) {
        item->public = true;
    }

    item->kind = AstItem_FuncDef;
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

        eat_tok_if(ctx, Token_Comma);
    }


    try (expect_tok(ctx, Token_RParen));
    try (expect_tok(ctx, Token_RArrow));

    item->func.sig.ret_type = try (parse_type(ctx));

    item->func.body = try (parse_block(ctx));

    return item;
}

bool parse_module(Module *module) {
    ParserCtx ctx;
    ctx.module = module;
    ctx.tok_idx = 0;

    while (!cur_tok_is(&ctx, Token_Eof)) {
        AstItem *item = try (parse_item(&ctx));
        dynarr_push(&ctx.module->ast, &item);
    }

    return true;
}
