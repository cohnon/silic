#include "fe/parser.h"

#include <string.h>

int main(void) {
    char *src =
        "use Silic.Log\n"
        "let msg = \"hello there\"\n"
        "let main () = {\n"
        "    Silic.log(msg);\n"
        "    Silic.hi(there);\n"
        "}\n"
    ;

    Parser prs = parser_init(src, strlen(src));
    Ast *ast = parser_parse(&prs);

    print_ast(stderr, ast);

    return 0;
}
