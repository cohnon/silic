#include "lexer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Lexer lexer_init(char *src, unsigned int len) {
    Lexer lxr;
    lxr.src = src;
    lxr.src_len = len;
    lxr.pos = 0;

    lexer_bump(&lxr);

    return lxr;
}

static char cur_char(Lexer *lxr) {
    return lxr->src[lxr->pos];
}

static char next_char(Lexer *lxr) {
    if (lxr->pos > lxr->src_len - 1) {
        return '\0';
    }
    return lxr->src[lxr->pos + 1];
}

static void skip_whitespace(Lexer *lxr) {
    while (
        cur_char(lxr) == ' ' ||
        cur_char(lxr) == '\t' ||
        cur_char(lxr) == '\r' ||
        cur_char(lxr) == '\n'
    ) {
        lxr->pos += 1;
    }
}

static void start_token(Lexer *lxr) {
    lxr->next_tok.src = lxr->src + lxr->pos;
}

static void end_token(Lexer *lxr, TokenKind kind) {
    lxr->pos += 1;

    lxr->next_tok.kind = kind;
    lxr->next_tok.len = (lxr->src + lxr->pos) - lxr->next_tok.src;
}

static void lex_dash(Lexer *lxr) {
    lxr->pos += 1;

    switch (cur_char(lxr)) {
        case '>': end_token(lxr, Token_ArrowRight); break;
        default: end_token(lxr, Token_Invalid); break;
    }
}

static _Bool tok_eq(Lexer *lxr, char *ident) {
    Token tok = lxr->next_tok;

    if (tok.len != strlen(ident)) { return 0; }

    return strncmp(tok.src, ident, tok.len) == 0;
}

static void lex_string(Lexer *lxr) {
    lxr->pos += 1;


    while (cur_char(lxr) != '"') {
        lxr->pos += 1;
    }

    end_token(lxr, Token_String);
}

static void lex_ident(Lexer *lxr) {
    _Bool uppercase = 'A' <= cur_char(lxr) && cur_char(lxr) <= 'Z';

    while (1) {
        _Bool alpha =
            ('a' <= next_char(lxr) && next_char(lxr) <= 'z') ||
            ('A' <= next_char(lxr) && next_char(lxr) <= 'Z');
        _Bool numeric = '0' <= next_char(lxr) && next_char(lxr) <= '9';
        
        if (!alpha && !numeric && next_char(lxr) != '_') {
            break;
        }

        lxr->pos += 1;
    }

    end_token(lxr, uppercase ? Token_IdentUpper : Token_IdentLower);

    if (tok_eq(lxr, "let")) {
        lxr->next_tok.kind = Token_Let;
    } else if (tok_eq(lxr, "use")) {
        lxr->next_tok.kind = Token_Use;
    }
}

static void lex(Lexer *lxr) {
    skip_whitespace(lxr);

    if (lxr->pos >= lxr->src_len) {
        end_token(lxr, Token_Eof);
        return;
    }

    start_token(lxr);

    switch (cur_char(lxr)) {
        case '+': end_token(lxr, Token_Plus); break;
        case '-': lex_dash(lxr); break;
        case '=': end_token(lxr, Token_Equal); break;

        case '(': end_token(lxr, Token_ParenOpen); break;
        case ')': end_token(lxr, Token_ParenClose); break;
        case '{': end_token(lxr, Token_BraceOpen); break;
        case '}': end_token(lxr, Token_BraceClose); break;
        
        case ',': end_token(lxr, Token_Comma); break;
        case '.': end_token(lxr, Token_Period); break;
        case ';': end_token(lxr, Token_Semicolon); break;
        case ':': end_token(lxr, Token_Colon); break;

        case '"': lex_string(lxr); break;

        default: {
            if ('a' <= cur_char(lxr) && cur_char(lxr) <= 'z') {
                lex_ident(lxr);
                break;
            }

            if ('A' <= cur_char(lxr) && cur_char(lxr) <= 'Z') {
                lex_ident(lxr);
                break;
            }

            end_token(lxr, Token_Invalid);
            break;
        }
    }
}

Token lexer_bump(Lexer *lxr) {
    Token cur_tok = lxr->next_tok;
    lex(lxr);
    return cur_tok;
}

Token lexer_peek(Lexer *lxr) {
    return lxr->next_tok;
}

Token lexer_expect(Lexer *lxr, TokenKind expected) {
    Token tok = lexer_bump(lxr);

    if (tok.kind != expected) {
        fprintf(stderr, "Lexer Error: expected %s, got %s\n", token_fmt(expected), token_fmt(tok.kind));
        exit(1);
    }

    return tok;
}

