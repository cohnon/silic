#include "lexer.h"

#include "token.h"
#include <assert.h>


#define ALPHA 'a': case 'b': case 'c': case 'd': case 'e': case \
              'f': case 'g': case 'h': case 'i': case 'j': case \
              'k': case 'l': case 'm': case 'n': case 'o': case \
              'p': case 'q': case 'r': case 's': case 't': case \
              'u': case 'v': case 'w': case 'x': case 'y': case 'z': \
         case 'A': case 'B': case 'C': case 'D': case 'E': case \
              'F': case 'G': case 'H': case 'I': case 'J': case \
              'K': case 'L': case 'M': case 'N': case 'O': case \
              'P': case 'Q': case 'R': case 'S': case 'T': case \
              'U': case 'V': case 'W': case 'X': case 'Y': case 'Z' \
              

#define DIGIT '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'

typedef struct LexerCtx {
    Module      *module;
    FirSym       src;
    size_t       src_idx;
    Token       *current_token;
    TextPos pos;
} LexerCtx;

static char cur_char(LexerCtx *ctx) {
    return ctx->src.ptr[ctx->src_idx];
}

static char next_char(LexerCtx *ctx) {
    return ctx->src.ptr[ctx->src_idx + 1];
}

static void inc(LexerCtx *ctx) {
    ctx->src_idx += 1;
    ctx->pos.col += 1;
}

static void inc2(LexerCtx *ctx) {
    ctx->src_idx += 2;
    ctx->pos.col += 2;
}

static void begin_token(LexerCtx *ctx, TokenKind kind) {
    Token *token = dynarr_add(&ctx->module->tokens);

    token->kind = kind;
    token->pos = ctx->pos;
    token->span.ptr = &ctx->module->src.ptr[ctx->src_idx];

    ctx->current_token = token;
}

typedef struct SymTokPair { char *symbol; TokenKind token; } SymTokPair;
const SymTokPair sym_tok_map[] = {
    { "use",         Token_Use         },
    { "unreachable", Token_Unreachable },
    { "as",          Token_As          },
    { "func",        Token_Func        },
    { "ret",         Token_Ret         },
    { "pub",         Token_Pub         },
};

static void end_token(LexerCtx *ctx) {
    Token *token = ctx->current_token;
    size_t start_idx = token->span.ptr - ctx->module->src.ptr;
    token->span.len = ctx->src_idx - start_idx;

    if (token->kind == Token_Symbol) {
        for (size_t i = 0; i < sizeof(sym_tok_map) / sizeof(sym_tok_map[0]); i += 1) {
            SymTokPair sym_tok = sym_tok_map[i];
            if (tok_eq_cstr(token, sym_tok.symbol)) {
                token->kind = sym_tok.token;
                break;
            }
        }
    }
}

static void skip_whitespace(LexerCtx *ctx) {
    while (ctx->src_idx < ctx->src.len && cur_char(ctx) <= ' ') {
        if (cur_char(ctx) == '\n') {
            ctx->pos.line += 1;
            // set to 0 since we inc right after
            ctx->pos.col= 0;
        }

        inc(ctx);
    }
}

static void lex_symbol(LexerCtx *ctx) {
    assert(('a' <= cur_char(ctx) && cur_char(ctx) <= 'z')
    ||     ('A' <= cur_char(ctx) && cur_char(ctx) <= 'Z')
    ||     (cur_char(ctx) == '_'));

    begin_token(ctx, Token_Symbol);

    inc(ctx);

    while (('a' <= cur_char(ctx) && cur_char(ctx) <= 'z')
    ||     ('A' <= cur_char(ctx) && cur_char(ctx) <= 'Z')
    ||     ('0' <= cur_char(ctx) && cur_char(ctx) <= '9')
    ||     cur_char(ctx) == '_' || cur_char(ctx) == '\'') {
        inc(ctx);
    }

    end_token(ctx);
}

static void lex_number(LexerCtx *ctx) {
    assert('0' <= cur_char(ctx) && cur_char(ctx) <= '9');

    begin_token(ctx, Token_Number);

    if (cur_char(ctx) == '0' && next_char(ctx) == 'b') {
        inc2(ctx);

        while (('0' <= cur_char(ctx) && cur_char(ctx) <= '1')
        ||     cur_char(ctx) == '_') {
            inc(ctx);
        }

    } else if (cur_char(ctx) == '0' && next_char(ctx) == 'o') {
        inc2(ctx);

        while (('0' <= cur_char(ctx) && cur_char(ctx) <= '7')
        ||     cur_char(ctx) == '_') {
            inc(ctx);
        }

    } else if (cur_char(ctx) == '0' && next_char(ctx) == 'x') {
        inc2(ctx);

        while (('0' <= cur_char(ctx) && cur_char(ctx) <= '9')
        ||     ('a' <= cur_char(ctx) && cur_char(ctx) <= 'f')
        ||     ('A' <= cur_char(ctx) && cur_char(ctx) <= 'F')
        ||      cur_char(ctx) == '_') {
            inc(ctx);
        }
    } else {
        inc(ctx);

        bool seen_decimal_point = false;

        while (true) {
            if (!seen_decimal_point && cur_char(ctx) == '.') {
                seen_decimal_point = true;
                inc(ctx);
                continue;
            }

            if (('0' <= cur_char(ctx) && cur_char(ctx) <= '9')
            ||   cur_char(ctx) == '_') {
                inc(ctx);
                continue;
            }

            break;
        }
    }

    end_token(ctx);
}

static void lex_string(LexerCtx *ctx) {
    assert(cur_char(ctx) == '"');

    begin_token(ctx, Token_String);

    inc(ctx);

    while (cur_char(ctx) != '"') {
        inc(ctx);
    }

    inc(ctx); // eat '"'

    end_token(ctx);
}

static void lex_dash(LexerCtx *ctx) {
    assert(cur_char(ctx) == '-');

    begin_token(ctx, Token_Dash);

    inc(ctx);

    if (cur_char(ctx) == '>') {
        inc(ctx);

        ctx->current_token->kind = Token_RArrow;
        end_token(ctx);

        return;
    }

    end_token(ctx);
}

static void lex_comment(LexerCtx *ctx) {
    assert(cur_char(ctx) == '/' || cur_char(ctx) == '*');

    ctx->current_token->kind = Token_String;

    if (cur_char(ctx) == '/') {
        while (cur_char(ctx) != '\n') {
            inc(ctx);
        }

        return;
    }

    // nested comment
    inc(ctx); // eat '*'

    int nested_lvl = 1;

    while (nested_lvl > 0) {
        if (cur_char(ctx) == '/' && next_char(ctx) == '*') {
            nested_lvl += 1;
            inc2(ctx);
        
        } else if (cur_char(ctx) == '*' && next_char(ctx) == '/') {
            nested_lvl -= 1;
            inc2(ctx);
        
        } else {
            inc(ctx);
        }
    }

    end_token(ctx);
}

static void lex_slash(LexerCtx *ctx) {
    assert(cur_char(ctx) == '/');

    begin_token(ctx, Token_Slash);

    inc(ctx);

    if (cur_char(ctx) == '/' || cur_char(ctx) == '*') {
        lex_comment(ctx);
        return;
    }

    end_token(ctx);
}
#include <stdio.h>
bool lex_module(Module *module) {
    LexerCtx ctx;
    ctx.module = module;
    ctx.src = module->src;
    ctx.src_idx = 0;
    ctx.current_token = NULL;
    ctx.pos = (TextPos){ 1, 1};

    skip_whitespace(&ctx);

    while (ctx.src_idx < ctx.src.len) {
        #define CASE_SINGLE_CHAR(ch, kind) case ch: \
            begin_token(&ctx, kind);                \
            inc(&ctx);                              \
            end_token(&ctx);                        \
            break

        switch (cur_char(&ctx)) {
        case ALPHA: case '_': lex_symbol(&ctx); break;
        case DIGIT:           lex_number(&ctx); break;
        case '"':             lex_string(&ctx); break;
        case '/':             lex_slash(&ctx); break;
        case '-':             lex_dash(&ctx); break;

        CASE_SINGLE_CHAR(';', Token_Semicolon);
        CASE_SINGLE_CHAR(',', Token_Comma);
        CASE_SINGLE_CHAR('{', Token_LBrace);
        CASE_SINGLE_CHAR('}', Token_RBrace);
        CASE_SINGLE_CHAR('(', Token_LParen);
        CASE_SINGLE_CHAR(')', Token_RParen);
        CASE_SINGLE_CHAR('!', Token_Bang);
        CASE_SINGLE_CHAR(':', Token_Colon);

        CASE_SINGLE_CHAR('.', Token_Dot);
        CASE_SINGLE_CHAR('&', Token_Ampersand);
        CASE_SINGLE_CHAR('%', Token_Percent);
        CASE_SINGLE_CHAR('+', Token_Plus);
        CASE_SINGLE_CHAR('=', Token_Equals);
        CASE_SINGLE_CHAR('>', Token_GreaterThan);
        CASE_SINGLE_CHAR('<', Token_LessThan);
        CASE_SINGLE_CHAR('~', Token_Tilde);
        CASE_SINGLE_CHAR('*', Token_Star);

        default: {
            //char unknown_char = cur_char(&ctx);
            begin_token(&ctx, Token_Invalid);
            inc(&ctx);
            end_token(&ctx);
            /**
            errors_new(
                &ctx.module->errors,
                ctx.module->path,
                ctx.current_token->position,
                ctx.current_token->span,
                "unexpected character '%c'",
                unknown_char
            );
            **/
            return false;
        }

        }

        skip_whitespace(&ctx);
    }

    begin_token(&ctx, Token_Eof);
    end_token(&ctx);

    return true;
}

