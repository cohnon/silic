#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "fe/lexer.h"

typedef struct Parser {
    Lexer lxr;
} Parser;

Parser parser_init(char *src, unsigned int len);

Ast *parser_parse(Parser *prs);

#endif
