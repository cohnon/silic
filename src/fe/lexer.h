#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct Lexer {
    char *src;
    unsigned int src_len;

    unsigned int pos;

    Token next_tok;
} Lexer;

Lexer lexer_init(char *src, unsigned int len);
Token lexer_bump(Lexer *lxr);
Token lexer_peek(Lexer *lxr);
Token lexer_expect(Lexer *lxr, TokenKind expected);

#endif
