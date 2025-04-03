#include "lexer.h"

#include <stdlib.h>
#include <string.h>

Lexer lexer_init(char *src, size_t len) {
    Lexer lexer;
    lexer.src = src;
    lexer.len = len;
    lexer.idx = 0;

    lexer_bump(&lexer);

    return lexer;
}

static char peek_char(Lexer *lexer) {
    return lexer->src[lexer->idx + 1];
}

static void make_token(Lexer *lexer, TokenKind kind) {
    lexer->next_tok.kind = kind;
    unsigned int tok_len = (lexer->src + lexer->idx) - lexer->next_tok.span.start;

    lexer->next_tok.span.len = tok_len;
}

static void skip_whitespace(Lexer *lexer) {
    while (1) {
        char ch = peek_char(lexer);

        if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
            lexer->idx += 1;
            continue;
        }

        break;
    }
}

static void lex_single(Lexer *lexer, TokenKind kind) {
    lexer->idx += 1;
    make_token(lexer, kind);
}

static void lex_dash(Lexer *lexer) {
    lexer->idx += 1;

    char ch = peek_char(lexer);

    switch (ch) {
        case '>':
            lexer->idx += 1;
            make_token(lexer, Token_Arrow);
            break;

        default:
            make_token(lexer, Token_Dash);
            break;
    }
}

static _Bool is_keyword(Lexer *lexer, const char *str) {
    Token tok = lexer->next_tok;

    size_t max_len = tok.span.len > strlen(str) ? tok.span.len : strlen(str);
    
    return strncmp(tok.span.start, str, max_len) == 0;
}

static void lex_letter(Lexer *lexer) {
    char first_ch = peek_char(lexer);

    lexer->idx += 1;

    while (1) {
        char ch = peek_char(lexer);

        _Bool lowercase = 'a' <= ch && ch <= 'z';
        _Bool uppercase = 'A' <= ch && ch <= 'Z';
        _Bool numeric = '0' <= ch && ch <= '9';

        if (!lowercase && !uppercase && !numeric) {
            break;
        }

        lexer->idx += 1;
    }

    TokenKind kind = 'a' <= first_ch && first_ch <= 'z'
        ? Token_IdentLower
        : Token_IdentUpper;

    make_token(lexer, kind);

    // check for keyword
    if (is_keyword(lexer, "proc")) {
        lexer->next_tok.kind = Token_Proc;
    } else if (is_keyword(lexer, "none")) {
        lexer->next_tok.kind = Token_None;
    }
}

Token lexer_bump(Lexer *lexer) {
    if (lexer->idx >= lexer->len) {
        return lexer->next_tok;
    }

    Token cur_tok = lexer->next_tok;

    // init new token
    lexer->next_tok.pos = lexer->pos;
    lexer->next_tok.span.start = lexer->src + lexer->idx;

    skip_whitespace(lexer);

    char ch = peek_char(lexer);
    switch (ch) {
        case 0: lex_single(lexer, Token_Eof); break;

        case ',': lex_single(lexer, Token_Comma); break;
        case ':': lex_single(lexer, Token_Colon); break;
        case '-': lex_dash(lexer); break;
        case '+': lex_single(lexer, Token_Plus); break;

        case '(': lex_single(lexer, Token_ParenOpen); break;
        case ')': lex_single(lexer, Token_ParenClose); break;
        case '{': lex_single(lexer, Token_BraceOpen); break;
        case '}': lex_single(lexer, Token_BraceClose); break;

        default:
            if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')) {
                lex_letter(lexer);
                break;
            }

            lex_single(lexer, Token_Invalid);
            break;
    }

    return cur_tok;
}
