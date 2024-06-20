#ifndef ERROR_MSG_H
#define ERROR_MSG_H

#include "span.h"
#include "token.h"

typedef unsigned int ErrorMsgId;

typedef enum ErrorMsgSeverity {
    ErrorMsgSeverity_Error,
    ErrorMsgSeverity_Warning,
} ErrorMsgSeverity;

typedef struct ErrorMsg {
    ErrorMsgSeverity severity;
    Span             msg;
    Span             hint;

    Span             span;

    size_t           line;
    size_t           col;
} ErrorMsg;

typedef struct Module Module;
__attribute__((format(printf, 3, 4)))
ErrorMsgId error_add(Module *module, Token *tok, char *msg, ...);

__attribute__((format(printf, 3, 4)))
void error_hint(Module *module, ErrorMsgId id, char *msg, ...);

void error_print(Module *module);

#endif
