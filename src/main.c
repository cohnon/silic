#include "parser/lexer.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    char *src =
        "use Std.Log\n"
        "Log.info(\"hey world\")\n"
    ;

    Lexer lxr = lexer_init(src, strlen(src));

    for (;;) {
        Token tok = lexer_bump(&lxr);

        if (tok.kind == Token_Eof) {
            break;
        }

        printf("%s ", token_fmt(tok.kind));
    }

    printf("\n");

    return 0;
}
