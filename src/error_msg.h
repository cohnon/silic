#ifndef ERROR_MSG_H
#define ERROR_MSG_H

#include "span.h"


typedef enum ErrorMsgSeverity {
    ErrorMsgSeverity_Error,
    ErrorMsgSeverity_Warning,
} ErrorMsgSeverity;

typedef struct ErrorMsg {
    ErrorMsgSeverity severity;
    Span             msg;
    Span             hint;

    int              line;
    int              col_start;
    int              col_end;
} ErrorMsg;

ErrorMsg error_msg_init(char *msg);
void error_msg_print(ErrorMsg *error_msg);

#endif
