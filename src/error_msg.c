#include "error_msg.h"

#include "compiler.h"
#include <fir/os.h>
#include <fir/dynarr.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


ErrorMsgId error_add(Compiler *compiler, ErrorMsgKind kind, String file_path, String span, TextPos pos) {
    String msg;
    switch (kind) {
    case ErrorMsg_SyntaxError: msg = string_lit("syntax error"); break;
    case ErrorMsg_NameConflict: msg = string_lit("name conflict"); break;
    }

    ErrorMsg error_msg = (ErrorMsg) {
        .severity  = ErrorMsgSeverity_Error,
        .msg       = msg,

        .file_path = file_path,

        .span      = span,
        .pos       = pos,
    };

    dynarr_push(&compiler->errors, &error_msg);

    return compiler->errors.len - 1;
}

void error_hint(Compiler *compiler, ErrorMsgId id, char *msg, ...) {
    size_t max_len = strlen(msg) + 256;
    char *formatted_msg = os_alloc(char, 256);

    va_list args;
    va_start(args, msg);
    vsnprintf(formatted_msg, max_len, msg, args);
    va_end(args);

    String hint = string_slc(formatted_msg, strlen(formatted_msg));

    ErrorMsg *error = dynarr_get_ref(&compiler->errors, id);
    error->hint = hint;
}

static void error_print_msg(String filepath, ErrorMsg *error) {
    // error: syntax error
    //   ╭─ path/to/file.sil
    // 5 │ io:println("hello world.")
    //   │                           ^ missing semicolon
    
    int line_num_width = 0;
    for (int i = error->pos.line; i > 0; i /= 10) { line_num_width += 1; }

    printf(ANSI_RED "error:" ANSI_RESET " %.*s\n" ANSI_RESET, string_fmt(error->msg));

    // print filepath
    printf(
        ANSI_GREY " %.*s ╭─ " ANSI_RESET "%.*s:%zu:%zu\n",
        line_num_width, "     ",
        string_fmt(filepath),
        error->pos.line,
        error->pos.col
    );

    // print source
    printf(ANSI_GREY " %zu │ " ANSI_RESET, error->pos.line);

    char *src = (char*)error->span.ptr - (error->pos.col- 1);
    while (*src != '\n' && *src != 0) {
        if (src == error->span.ptr) { printf(ANSI_RED); }
        putc(*src, stdout);
        if(src == error->span.ptr + error->span.len - 1) { printf(ANSI_RESET); }
        src += 1;
    }

    putc('\n', stdout);

    // print hint
    printf(
        ANSI_GREY " %.*s │ " ANSI_RED,
        line_num_width, "     "
    );

    for (size_t i = 0; i < error->pos.col- 1; i += 1) { putc(' ', stdout); }
    for (size_t i = 0; i < error->span.len; i += 1) { putc('^', stdout); }
    
    printf(" %.*s", string_fmt(error->hint));


    printf("\n\n" ANSI_RESET);
}

void error_print(Compiler *compiler) {
    dynarr_foreach(compiler->errors, i) {
        ErrorMsg *error = dynarr_get_ref(&compiler->errors, i);
        error_print_msg(error->file_path, error);
    }
}
