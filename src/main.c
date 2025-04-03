#include "parser/lexer.h"
#include <stdio.h>
#include <stdlib.h>

void print_usage(char *arg0) {
    printf("usage: %s <entry_point>\n", arg0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    char *entry_point_path = argv[1];
    FILE *entry_point = fopen(entry_point_path, "rb");
    if (entry_point == NULL) {
        printf("couldn't find %s", entry_point_path);
        return 1;
    }

    fseek(entry_point, 0, SEEK_END);
    long len = ftell(entry_point);
    char *buf = malloc(len);

    fseek(entry_point, 0, SEEK_SET);
    fread(buf, 1, len, entry_point);

    fclose(entry_point);

    Lexer lexer = lexer_init(buf, len);

    while (1) {
        Token tok = lexer_bump(&lexer);

        if (tok.kind == Token_Eof) { break; }

        printf("%s ", token_str(tok));
    }
    printf("\n");

    return 0;
}
