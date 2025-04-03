#include "token.h"

#include <stdlib.h>

char *token_str(Token tok) {
    switch (tok.kind) {
        case Token_Invalid: return "Invalid";

        case Token_Eof: return "Eof";

        case Token_IdentLower: return "IdentLower";
        case Token_IdentUpper: return "IdentUpper";

        case Token_Comma: return ",";
        case Token_Colon: return ":";
        case Token_Dash: return "-";
        case Token_Arrow: return "->";
        case Token_Plus: return "+";

        case Token_ParenOpen: return "(";
        case Token_ParenClose: return ")";
        case Token_BraceOpen: return "{";
        case Token_BraceClose: return "}";

        case Token_Proc: return "proc";
        case Token_None: return "none";
    }

    return NULL;
}

