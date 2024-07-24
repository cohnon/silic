#include "compiler.h"
#include <fir/fir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("[invalid commands] usage: %s <path to entry point>\n", argv[0]);

        return EXIT_FAILURE;
    }

    String entry_path = string_slc(argv[1], strlen(argv[1]));

    Compiler *compiler = compiler_init();

    bool compiled = compiler_compile(compiler, entry_path);

    if (!compiled) {
        printf("failed to compile\n");
        return 1;
    }

    return 0;
}
