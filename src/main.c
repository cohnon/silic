#include "parser/lexer.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    char *src = "func add(a: Int, b: Int) -> Int { a + b }";

    Lexer lxr = lexer_init(src, strlen(src));

    for (;;) {
        Token tok = lexer_bump(&lxr);

        if (tok.kind == Token_Eof) {
            break;
        }

        printf("%s ", token_fmt(tok.kind));
    }

    return 0;
}
