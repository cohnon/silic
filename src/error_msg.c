#include "error_msg.h"

#include "array.h"
#include "module.h"
#include "os.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#define ANSI_RESET "\033[0m"
#define ANSI_RED   "\033[1;31m"
#define ANSI_BLUE  "\033[34m"
#define ANSI_GREY  "\033[90m"

ErrorMsgId error_add(Module *module, Token *tok, char *msg, ...) {
    size_t max_len = strlen(msg) + 256;
    char *formatted_msg = os_alloc(char, 256);

    va_list args;
    va_start(args, msg);
    vsnprintf(formatted_msg, max_len, msg, args);
    va_end(args);

    Span msg_span = (Span){
        .ptr = formatted_msg,
        .len = strlen(formatted_msg)
    };

    ErrorMsg error_msg = (ErrorMsg) {
        .msg       = msg_span,
        .span      = tok->span,
        .col       = tok->pos.col,
        .line      = tok->pos.line,
        .severity  = ErrorMsgSeverity_Error,
    };

    array_push(&module->errors, &error_msg);

    return module->errors.len - 1;
}

void error_hint(Module *module, ErrorMsgId id, char *msg, ...) {
    size_t max_len = strlen(msg) + 256;
    char *formatted_msg = os_alloc(char, 256);

    va_list args;
    va_start(args, msg);
    vsnprintf(formatted_msg, max_len, msg, args);
    va_end(args);

    Span hint = (Span){
        .ptr = formatted_msg,
        .len = strlen(formatted_msg)
    };

    printf("HINT LEN: %zu", hint.len);

    ErrorMsg *error = array_get_ref(&module->errors, id);
    error->hint = hint;
}

static void error_print_msg(Span filepath, ErrorMsg *error) {
    // error: syntax error
    //   ╭─ path/to/file.sil
    // 5 │ io:println("hello world.")
    //   │                           ^ missing semicolon
    
    int line_num_width = 0;
    for (int i = error->line; i > 0; i /= 10) { line_num_width += 1; }

    printf(ANSI_RED "error:" ANSI_RESET " %.*s\n" ANSI_RESET, span_fmt(error->msg));

    // print filepath
    printf(
        ANSI_GREY " %.*s ╭─ " ANSI_RESET "%.*s:%zu:%zu\n",
        line_num_width, "     ",
        span_fmt(filepath),
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
    
    printf(" %.*s", span_fmt(error->hint));


    printf("\n\n" ANSI_RESET);
}

void error_print(Module *module) {
    array_foreach(module->errors, i) {
        ErrorMsg *error = array_get_ref(&module->errors, i);
        error_print_msg(module->filepath, error);
    }
}
