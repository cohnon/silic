#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenKind {
    Token_Invalid,
    Token_Eof,

    // operators
    Token_Plus,
    Token_Dash,
    Token_Star,
    Token_Slash,
    Token_Equal,

    // structural
    Token_ParenOpen,
    Token_ParenClose,
    Token_BraceOpen,
    Token_BraceClose,
    
    Token_Comma,
    Token_Period,
    Token_Semicolon,
    Token_Colon,

    Token_ArrowRight,

    // identifiers
    Token_IdentUpper,
    Token_IdentLower,

    // literals
    Token_String,

    // keywords
    Token_Let,
    Token_Use,
} TokenKind;

typedef struct Token {
    TokenKind kind;
    unsigned int len;
    char *src;
} Token;

char *token_string(TokenKind kind);
#define token_fmt(tok) tok.len, tok.src

#endif
