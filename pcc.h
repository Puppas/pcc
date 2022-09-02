#ifndef PCC_H
#define PCC_H

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

typedef enum {
    TK_PUNCT,   // Punctuators
    TK_IDENT,   // Identifiers
    TK_KEYWORD, // Keywords
    TK_NUM, 
    TK_EOF
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val; // used if kind is TK_NUM
    char *loc;
    int len;
};


void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);


//
// parse.c
//

typedef struct Node Node;

// local variable
typedef struct Obj Obj;
struct Obj
{
    Obj *next;
    char *name;     // variable name
    int offset;     
};

typedef struct Function Function;
struct Function
{
    Node *body;
    Obj *locals;
    int stack_size;
};



typedef enum {
    ND_ADD,         // + 
    ND_SUB,         // -
    ND_MUL,         // *
    ND_DIV,         // /
    ND_NEG,         // unary -
    ND_EQ,          // ==
    ND_NE,          // !=
    ND_LT,          // <
    ND_LE,          // <=
    ND_ASSIGN,      // =
    ND_RETURN,      // return
    ND_BLOCK,       // {...}
    ND_EXPR_STMT,   // expression statement
    ND_VAR,         // variable
    ND_NUM
} NodeKind;


struct Node {
    NodeKind kind;  
    Node *next;
    Node *lhs;
    Node *rhs;

    // block
    Node *body;     

    Obj *var;       // used if kind == ND_VAR
    int val;        // used if kind == ND_NUM
};


Function *parse(Token *tok);


//
// codegen.c
//

void codegen(Function *prog);


#endif /* PCC_H */
