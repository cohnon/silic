#include "error_msg.h"

#include <stdio.h>
#include <string.h>


ErrorMsg error_msg_init(char *msg) {
    Span msg_span = (Span){ .ptr = msg, .len = strlen(msg) };
    return (ErrorMsg) {
        .msg = msg_span,
    };
}

void error_msg_print(ErrorMsg *error_msg) {
    printf("error: %.*s\n", span_fmt(error_msg->msg));
}
