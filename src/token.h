#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenKind {
    Token_Invalid,
    Token_Eof,

    // operators
    Token_Plus,

    // structural
    Token_ParenOpen,
    Token_ParenClose,
    Token_BraceOpen,
    Token_BraceClose,
    
    Token_Comma,
    Token_Period,
    Token_Colon,

    Token_ArrowRight,

    // identifiers
    Token_IdentUpper,
    Token_IdentLower,

    // literals
    Token_String,

    // keywords
    Token_Use,
} TokenKind;

typedef struct Token {
    TokenKind kind;
    char *src;
    unsigned int len;
} Token;

char *token_fmt(TokenKind kind);

#endif
