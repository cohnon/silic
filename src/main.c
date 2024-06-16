#include "os.h"

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

    Span entry_src;
    bool read_file = os_read_file(&entry_src, entry_path);

    if (!read_file) {
        printf("couldn't find %s\n", argv[1]);

        return EXIT_FAILURE;
    }

    Module module = (Module){0};
    module.src = entry_src;

    bool lex_passed = lex_module(&module);
    if (!lex_passed) {
        printf("ERROR: failed to lex module\n");
        return 1;
    }

    for (size_t i = 0; i < module.tokens.len; i += 1) {
        printf("%s ", tok_cstr(array_get_ref(&module.tokens, i)));
    }
    printf("\n");

    bool parse_passed = parse_module(&module);
    if (!parse_passed) {
        printf("ERROR: failed to parse module\n");
        return 1;
    }

    return 0;
}
