#include "parser.h"

#include "ast.h"
#include "os.h"
#include "try.h"


typedef struct ParserCtx {
    Module *module;
    size_t tok_idx;
} ParserCtx;

static Token *cur_tok(ParserCtx *ctx) {
    return array_get_ref(&ctx->module->tokens, ctx->tok_idx);
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
    Token *tok = eat_tok(ctx);
    
    if (tok->kind != kind) {
        return NULL;
    }

    return tok;
}

static AstItem *parse_item(ParserCtx *ctx) {
    AstItem *item = os_alloc(AstItem);

    item->public = false;
    if (eat_tok_if(ctx, Token_Pub)) {
        item->public = true;
    }

    try (expect_tok(ctx, Token_Func));

    Token *func_name_tok = try (expect_tok(ctx, Token_Symbol));
    item->name = func_name_tok->span;

    try (expect_tok(ctx, Token_LParen));

    while (!cur_tok_is(ctx, Token_RParen)) {
        try (expect_tok(ctx, Token_Symbol));
        try (expect_tok(ctx, Token_Colon));
        try (expect_tok(ctx, Token_Symbol));

        eat_tok_if(ctx, Token_Comma);
    }


    try (expect_tok(ctx, Token_RParen));
    try (expect_tok(ctx, Token_RArrow));
    try (expect_tok(ctx, Token_Symbol));

    try (expect_tok(ctx, Token_LBrace));
    try (expect_tok(ctx, Token_RBrace));

    return item;
}

bool parse_module(Module *module) {
    ParserCtx ctx;
    ctx.module = module;
    ctx.tok_idx = 0;

    while (!cur_tok_is(&ctx, Token_Eof)) {
        try (parse_item(&ctx));
    }

    return true;
}
