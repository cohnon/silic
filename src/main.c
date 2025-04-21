#include "fe/parser.h"

#include <string.h>

int main(void) {
    char *src =
        "let add (a, b) = a + b\n"
    ;

    Parser prs = parser_init(src, strlen(src));
    Ast *ast = parser_parse(&prs);

    print_ast(stderr, ast);

    return 0;
}
