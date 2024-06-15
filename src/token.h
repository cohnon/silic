#ifndef TOKEN_H
#define TOKEN_H

#include "span.h"
#include <stdbool.h>


typedef struct TextPosition {
    size_t col;
    size_t row;
} TextPosition;

typedef enum TokenKind {
    Token_Invalid,

    Token_Number,
    Token_String,

    Token_Symbol,

    Token_Colon,
    Token_Semicolon,
    Token_Bang,
    Token_Dot,
    Token_Comma,
    Token_Ampersand,

    Token_LParen,
    Token_RParen,
    Token_LBrace,
    Token_RBrace,
    Token_RArrow,

    Token_Equals,
    Token_GreaterThan,
    Token_LessThan,
    Token_GreaterThanEquals,
    Token_LessThanEquals,

    Token_Tilde,
    Token_Plus,
    Token_Dash,
    Token_Star,
    Token_Slash,
    Token_Percent,

    Token_Use,
    Token_Unreachable,
    Token_As,
    Token_Func,
    Token_Ret,
    Token_Pub,

    Token_Eof,
} TokenKind;

typedef struct Token {
    TokenKind    kind;
    Span         span;
    TextPosition pos;
} Token;

bool tok_eq(Token *a, Token *b);
bool tok_eq_cstr(Token *a, const char *cstr);
char *tok_cstr(Token *token);

#endif
