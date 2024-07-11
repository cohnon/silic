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
    ErrorMsg_NameConflict,
} ErrorMsgKind;

typedef struct ErrorMsg {
    ErrorMsgSeverity severity;
    FirString        msg;
    FirString        hint;

    FirString        file_path;

    FirString        span;
    TextPos          pos;
} ErrorMsg;


typedef struct Compiler Compiler;
ErrorMsgId error_add(Compiler *compiler, ErrorMsgKind kind, FirString file_path, FirString span, TextPos pos);

__attribute__((format(printf, 3, 4)))
void error_hint(Compiler *compiler, ErrorMsgId id, char *msg, ...);

void error_print(Compiler *compiler);

#endif
