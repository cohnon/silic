#ifndef LEXER_H
#define LEXER_H

#include "compiler.h"
#include <stdbool.h>


bool lex_module(Compiler *compiler, Module *module);

#endif
