#include "error_msg.h"

#include "dynarr.h"
#include "module.h"
#include "os.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


ErrorMsgId error_add(Module *module, Token *tok, char *msg, ...) {
    size_t max_len = strlen(msg) + 256;
    char *formatted_msg = os_alloc(char, 256);

    va_list args;
    va_start(args, msg);
    vsnprintf(formatted_msg, max_len, msg, args);
    va_end(args);

    FirString msg_span = fir_string_slc(formatted_msg, strlen(formatted_msg));

    ErrorMsg error_msg = (ErrorMsg) {
        .msg       = msg_span,
        .span      = tok->span,
        .col       = tok->pos.col,
        .line      = tok->pos.line,
        .severity  = ErrorMsgSeverity_Error,
    };

    dynarr_push(&module->errors, &error_msg);

    return module->errors.len - 1;
}

void error_hint(Module *module, ErrorMsgId id, char *msg, ...) {
    size_t max_len = strlen(msg) + 256;
    char *formatted_msg = os_alloc(char, 256);

    va_list args;
    va_start(args, msg);
    vsnprintf(formatted_msg, max_len, msg, args);
    va_end(args);

    FirString hint = fir_string_slc(formatted_msg, strlen(formatted_msg));

    ErrorMsg *error = dynarr_get_ref(&module->errors, id);
    error->hint = hint;
}

static void error_print_msg(FirString filepath, ErrorMsg *error) {
    // error: syntax error
    //   ╭─ path/to/file.sil
    // 5 │ io:println("hello world.")
    //   │                           ^ missing semicolon
    
    int line_num_width = 0;
    for (int i = error->line; i > 0; i /= 10) { line_num_width += 1; }

    printf(ANSI_RED "error:" ANSI_RESET " %.*s\n" ANSI_RESET, fir_string_fmt(error->msg));

    // print filepath
    printf(
        ANSI_GREY " %.*s ╭─ " ANSI_RESET "%.*s:%zu:%zu\n",
        line_num_width, "     ",
        fir_string_fmt(filepath),
        error->line,
        error->col
    );

    // print source
    printf(ANSI_GREY " %zu │ " ANSI_RESET, error->line);

    char *src = (char*)error->span.ptr - (error->col- 1);
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

    for (size_t i = 0; i < error->col- 1; i += 1) { putc(' ', stdout); }
    for (size_t i = 0; i < error->span.len; i += 1) { putc('^', stdout); }
    
    printf(" %.*s", fir_string_fmt(error->hint));


    printf("\n\n" ANSI_RESET);
}

void error_print(Module *module) {
    dynarr_foreach(module->errors, i) {
        ErrorMsg *error = dynarr_get_ref(&module->errors, i);
        error_print_msg(module->filepath, error);
    }
}
