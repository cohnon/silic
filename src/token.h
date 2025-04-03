#ifndef TOKEN_H
#define TOKEN_H

typedef struct Position {
    unsigned int row;
    unsigned int col;
} Position;

typedef struct Span {
    char *start;
    unsigned int len;
} Span;

typedef enum TokenKind {
    Token_Invalid,

    Token_Eof,

    Token_IdentLower,
    Token_IdentUpper,

    Token_Comma,
    Token_Colon,
    Token_Dash,
    Token_Arrow,
    Token_Plus,

    Token_ParenOpen,
    Token_ParenClose,
    Token_BraceOpen,
    Token_BraceClose,

    // keywords
    Token_Proc,
    Token_None,
} TokenKind;

typedef struct Token {
    TokenKind kind;

    Position pos;
    Span span;
} Token;

char *token_str(Token tok);

#endif
