#ifndef ERROR_MSG_H
#define ERROR_MSG_H

#include "token.h"

typedef unsigned int ErrorMsgId;

typedef enum ErrorMsgSeverity {
    ErrorMsgSeverity_Error,
    ErrorMsgSeverity_Warning,
} ErrorMsgSeverity;

typedef enum ErrorMsgKind {
    ErrorMsg_SyntaxError,
} ErrorMsgKind;

typedef struct ErrorMsg {
    ErrorMsgSeverity severity;
    FirString        msg;
    FirString        hint;

    FirString        span;
    TextPos          pos;
} ErrorMsg;


typedef struct Module Module;
ErrorMsgId error_add(Module *module, ErrorMsgKind kind, FirString span, TextPos pos);

__attribute__((format(printf, 3, 4)))
void error_hint(Module *module, ErrorMsgId id, char *msg, ...);

void error_print(Module *module);

#endif
