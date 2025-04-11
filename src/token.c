#include "token.h"

char *token_fmt(TokenKind kind) {
    switch (kind) {
        case Token_Invalid: return "<invalid>";
        case Token_Eof: return "<eof>";
        case Token_Plus: return "+";
        case Token_BraceOpen: return "{";
        case Token_BraceClose: return "}";
        case Token_ParenOpen: return "(";
        case Token_ParenClose: return ")";
        case Token_Comma: return ",";
        case Token_Period: return ".";
        case Token_Colon: return ":";
        case Token_ArrowRight: return "->";
        case Token_IdentUpper: return "<Ident>";
        case Token_IdentLower: return "<ident>";
        case Token_String: return "<string>";
        case Token_Use: return "use";
    }

    __builtin_unreachable();
}
