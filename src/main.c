#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("[invalid commands] usage: %s <path to entry point>\n", argv[0]);

        return EXIT_FAILURE;
    }

    Span entry_path = (Span){ argv[1], strlen(argv[1]) };
    Module *module = module_init(entry_path);
    if (module == NULL) {
        return 1;
    }

    bool lex_passed = lex_module(module);
    if (!lex_passed) {
        printf("ERROR: failed to lex module\n");
        return 1;
    }

    tok_debug(module);

    bool parse_passed = parse_module(module);
    if (!parse_passed) {
        printf("ERROR: failed to parse module\n");
        error_print(module);
        return 1;
    }

    ast_debug(module);

    return 0;
}
