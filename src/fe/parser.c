#include "parser.h"

#include "ast.h"
#include "fe/lexer.h"
#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Parser {
    Lexer lxr;
} Parser;

static AstPattern *parse_pattern(Parser *prs) {
    AstPattern *pat = malloc(sizeof(AstPattern));
    pat->ident = lexer_expect(&prs->lxr, Token_IdentLower);

    return pat;
}

static AstExpr *parse_expression(Parser *prs);

static AstExpr *parse_block(Parser *prs) {
    AstExpr *expr = malloc(sizeof(AstExpr));
    expr->kind = AstExpr_Block;
    expr->block.exprs = list_init(AstExpr *, 16);

    lexer_expect(&prs->lxr, Token_BraceOpen);

    while (lexer_peek(&prs->lxr).kind != Token_BraceClose) {
        list_push(&expr->block.exprs, AstExpr *, parse_expression(prs));
        
        if (lexer_peek(&prs->lxr).kind == Token_Semicolon) {
            lexer_bump(&prs->lxr);
        } else {
            break;
        }

    }

    lexer_expect(&prs->lxr, Token_BraceClose);

    return expr;
}

static AstExpr *parse_identifier(Parser *prs) { 
    AstExpr *expr = malloc(sizeof(AstExpr));
    expr->kind = AstExpr_Identifier;
    expr->ident.tok = lexer_bump(&prs->lxr);
    assert(
        expr->ident.tok.kind == Token_IdentLower ||
        expr->ident.tok.kind == Token_IdentUpper
    );

    if (lexer_peek(&prs->lxr).kind == Token_Period) {
        lexer_bump(&prs->lxr); // .
        AstExpr *mem_expr = malloc(sizeof(AstExpr));
        mem_expr->kind = AstExpr_MemberAccess;
        mem_expr->mem_access.target = expr;
        mem_expr->mem_access.member = parse_identifier(prs);

        return mem_expr;
    }

    return expr;
}

static AstExpr *parse_string_literal(Parser *prs) {
    AstExpr *expr = malloc(sizeof(AstExpr *));
    expr->kind = AstExpr_String;
    expr->string.tok = lexer_expect(&prs->lxr, Token_String);

    return expr;
}

static AstExpr *parse_function_call(Parser *prs, AstExpr *target) {
    AstExpr *expr = malloc(sizeof(AstExpr));
    expr->kind = AstExpr_FunctionCall;
    expr->fn_call.target = target;
    expr->fn_call.args = list_init(AstExpr*, 6);

    lexer_expect(&prs->lxr, Token_ParenOpen);

    while (lexer_peek(&prs->lxr).kind != Token_ParenClose) {
        AstExpr *arg = parse_expression(prs);
        list_push(&expr->fn_call.args, AstExpr *, arg);

        if (lexer_peek(&prs->lxr).kind == Token_Comma) {
            lexer_bump(&prs->lxr);
        } else {
            break;
        }

    }

    lexer_expect(&prs->lxr, Token_ParenClose);

    return expr;
}

static AstExpr *parse_expression_suffix(Parser *prs, AstExpr *target) {
    for (;;) {
        switch (lexer_peek(&prs->lxr).kind) {
            case Token_ParenOpen: target = parse_function_call(prs, target); break;
            default: return target;
        }
    }
}

static AstExpr *parse_expression_primary(Parser *prs) {
    AstExpr *expr = NULL;

    switch (lexer_peek(&prs->lxr).kind) {
        case Token_ParenOpen:
            lexer_bump(&prs->lxr); // (
            parse_expression(prs);
            lexer_expect(&prs->lxr, Token_ParenClose);
            break;

        case Token_BraceOpen: expr = parse_block(prs); break;

        case Token_IdentLower:
        case Token_IdentUpper: expr = parse_identifier(prs); break;

        case Token_String: expr = parse_string_literal(prs); break;
        default:
            fprintf(
                stderr,
                "unexpected expression token %s\n",
                token_fmt(lexer_peek(&prs->lxr).kind)
            );
            exit(1);
    }

    return parse_expression_suffix(prs, expr);
}

typedef struct Operator {
    AstExprBinOpKind kind;
    int prec;
} Operator;

static Operator parse_operator(Parser *prs) {
    switch (lexer_peek(&prs->lxr).kind) {
        case Token_Plus: return (Operator){ AstExprBinOp_Add, 11 };
        case Token_Dash: return (Operator){ AstExprBinOp_Sub, 11 };
        case Token_Star: return (Operator){ AstExprBinOp_Mul, 12 };
        case Token_Slash: return (Operator){ AstExprBinOp_Div, 12 };

        default: return (Operator){ 0, -1 };
    }
}

static AstExpr *parse_expression_prec(Parser *prs, int prec) {
    AstExpr *lhs = parse_expression_primary(prs);

    for (;;) {
        Operator op = parse_operator(prs);

        // no operator
        if (op.prec == -1) { break; }

        // if next operator doesn't exceed current precedence
        // break out and wrap expression so far
        // 1 + 2 + 3 * 4
        //       ^ has same precedence so wrap previous expression (1 + 2)
        if (op.prec <= prec) { break; }

        lexer_bump(&prs->lxr); // operator

        AstExpr *rhs = parse_expression_prec(prs, op.prec);
        
        AstExpr *op_expr = malloc(sizeof(AstExpr));
        op_expr->kind = AstExpr_BinOp;
        op_expr->bin_op.kind = op.kind;
        op_expr->bin_op.lhs = lhs;
        op_expr->bin_op.rhs = rhs;

        lhs = op_expr;
    }

    return lhs;
}

static AstExpr *parse_expression(Parser *prs) {
    return parse_expression_prec(prs, 0);
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

    stmt->let.pat = parse_pattern(prs);

    // function parameters
    stmt->let.is_fn = false;
    if (lexer_peek(&prs->lxr).kind != Token_Equal) {
        stmt->let.is_fn = true;
        stmt->let.params = list_init(AstPattern *, 8);

        lexer_expect(&prs->lxr, Token_ParenOpen);

        while (lexer_peek(&prs->lxr).kind != Token_ParenClose) {
            list_push(&stmt->let.params, AstPattern *, parse_pattern(prs));

            if (lexer_peek(&prs->lxr).kind == Token_Comma) {
                lexer_bump(&prs->lxr);
            } else {
                break;
            }

        }

        lexer_expect(&prs->lxr, Token_ParenClose);
    }

    lexer_expect(&prs->lxr, Token_Equal);

    stmt->let.expr = parse_expression(prs);

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

Ast *parser_parse(char *src) {
    Parser prs;
    prs.lxr = lexer_init(src, strlen(src));

    Ast *ast = malloc(sizeof(Ast));
    ast->stmts = list_init(AstStmt, 32);

    for (;;) {
        switch (lexer_peek(&prs.lxr).kind) {
            case Token_Eof: return ast;

            default: list_push(&ast->stmts, AstStmt *, parse_top_level_statement(&prs));
        }
    }
}
