#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stddef.h>

typedef struct Lexer {
    char *src;
    size_t len;

    size_t idx;
    Position pos;

    Token next_tok;
} Lexer;

Lexer lexer_init(char *src, size_t len);
Token lexer_bump(Lexer *lexer);

#endif
