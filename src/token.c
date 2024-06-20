#include "token.h"

#include <string.h>


bool tok_eq(Token *a, Token *b) {
    size_t max_len = a->span.len > b->span.len ? a->span.len : b->span.len;
    return strncmp(a->span.ptr, b->span.ptr, max_len) == 0;
}

bool tok_eq_cstr(Token *a, const char *cstr) {
    size_t cstr_len = strlen(cstr);
    size_t max_len = a->span.len > cstr_len ? a->span.len : cstr_len;
    return strncmp(a->span.ptr, cstr, max_len) == 0;
}

char *tok_cstr(TokenKind kind) {
    switch (kind) {
    case Token_Invalid: return "<invalid>";

    case Token_Number: return "number literal";
    case Token_String: return "string literal";

    case Token_Symbol: return "symbol";

    case Token_Colon: return ":";
    case Token_Semicolon: return ";";
    case Token_Bang: return "!";
    case Token_Dot: return ".";
    case Token_Comma: return ",";
    case Token_Ampersand: return "&";

    case Token_LParen: return "(";
    case Token_RParen: return ")";
    case Token_LBrace: return "{";
    case Token_RBrace: return "}";
    case Token_RArrow: return "->";

    case Token_Equals: return "=";
    case Token_GreaterThan: return ">";
    case Token_LessThan: return "<";
    case Token_LessThanEquals: return "<=";
    case Token_GreaterThanEquals: return ">=";

    case Token_Tilde: return "~";
    case Token_Plus: return "+";
    case Token_Dash: return "-";
    case Token_Star: return "*";
    case Token_Slash: return "/";
    case Token_Percent: return "%";

    case Token_Use: return "use";
    case Token_Unreachable: return "unreachable";
    case Token_As: return "as";
    case Token_Func: return "func";
    case Token_Ret: return "ret";
    case Token_Pub: return "pub";

    case Token_Eof: return "EOF";
    }

    __builtin_unreachable();
}

