#include "parser.h"

#include "ast.h"
#include "fe/lexer.h"
#include <stdio.h>
#include <stdlib.h>

Parser parser_init(char *src, unsigned int len) {
    Parser prs;
    prs.lxr = lexer_init(src, len);

    return prs;
}

static AstExpr *parse_expression(Parser *prs);

static AstExpr *parse_block(Parser *prs) {
    AstExpr *expr = malloc(sizeof(AstExpr));

    lexer_expect(&prs->lxr, Token_BraceOpen);

    while (lexer_peek(&prs->lxr).kind != Token_BraceClose) {
        parse_expression(prs);
        
        if (lexer_peek(&prs->lxr).kind != Token_Semicolon) {
            break;
        }

        lexer_bump(&prs->lxr); // ;
    }

    lexer_expect(&prs->lxr, Token_BraceClose);

    return expr;
}

static AstExpr *parse_identifier(Parser *prs) { 
    AstExpr *expr = malloc(sizeof(AstExpr));

    lexer_expect(&prs->lxr, Token_IdentLower);

    if (lexer_peek(&prs->lxr).kind == Token_ParenOpen) {
        lexer_bump(&prs->lxr); // (
        parse_expression(prs);
        lexer_expect(&prs->lxr, Token_ParenClose);
    }


    return expr;
}

static AstExpr *parse_module_access(Parser *prs) {
    AstExpr *expr = malloc(sizeof(AstExpr));
    expr->kind = AstExpr_ModuleAccess;

    lexer_expect(&prs->lxr, Token_IdentUpper);
    lexer_expect(&prs->lxr, Token_Period);
    parse_identifier(prs);

    return expr;
}

static AstExpr *parse_string_literal(Parser *prs) {
    AstExpr *expr = malloc(sizeof(AstExpr *));
    expr->kind = AstExpr_String;
    expr->string = lexer_expect(&prs->lxr, Token_String);

    return expr;
}


static AstExpr *parse_expression(Parser *prs) {
    switch (lexer_peek(&prs->lxr).kind) {
        case Token_BraceOpen: return parse_block(prs);
        case Token_IdentLower: return parse_identifier(prs);
        case Token_IdentUpper: return parse_module_access(prs);
        case Token_String: return parse_string_literal(prs);
        default:
            fprintf(
                stderr,
                "unexpected expression %s\n",
                token_fmt(lexer_peek(&prs->lxr).kind)
            );
            exit(1);
    }
}

static AstStmt *parse_use(Parser *prs) {
    AstStmt *stmt = malloc(sizeof(AstStmt));
    stmt->kind = AstStmt_Use;

    lexer_expect(&prs->lxr, Token_Use);

    for (;;) {
        lexer_expect(&prs->lxr, Token_IdentUpper);

        if (lexer_peek(&prs->lxr).kind != Token_Period) {
            break;
        }

        lexer_expect(&prs->lxr, Token_Period);
    }

    return stmt;
}

static AstStmt *parse_let(Parser *prs) {
    AstStmt *stmt = malloc(sizeof(AstStmt));
    stmt->kind = AstStmt_Let;

    lexer_expect(&prs->lxr, Token_Let);

    lexer_expect(&prs->lxr, Token_IdentLower);

    // function parameters
    if (lexer_peek(&prs->lxr).kind != Token_Equal) {
        lexer_expect(&prs->lxr, Token_ParenOpen);
        lexer_expect(&prs->lxr, Token_ParenClose);
    }

    lexer_expect(&prs->lxr, Token_Equal);

    stmt->expr = parse_expression(prs);

    return stmt;
}

/*static AstStmt *parse_statement_expression(Parser *prs) {
    AstStmt *stmt = malloc(sizeof(AstStmt));
    stmt->kind = AstStmt_Expr;

    parse_expression(prs);

    return stmt;
}

static AstStmt *parse_statement(Parser *prs) {
    switch (lexer_peek(&prs->lxr).kind) {
        case Token_Use: return parse_use(prs);
        case Token_Let: return parse_let(prs);
        default: return parse_statement_expression(prs);
    }
}*/

static AstStmt *parse_top_level_statement(Parser *prs) {
    switch (lexer_peek(&prs->lxr).kind) {
        case Token_Use: return parse_use(prs);
        case Token_Let: return parse_let(prs);
        default:
            fprintf(
                stderr,
                "unexpected top level statement: %s\n",
                token_fmt(lexer_peek(&prs->lxr).kind)
            );
            exit(1);
    }
}

Ast *parser_parse(Parser *prs) {
    Ast *ast = malloc(sizeof(Ast));
    ast->stmts = list_init(AstStmt, 32);

    for (;;) {
        switch (lexer_peek(&prs->lxr).kind) {
            case Token_Eof: return ast;

            default: list_push(&ast->stmts, AstStmt *, parse_top_level_statement(prs));
        }
    }
}
