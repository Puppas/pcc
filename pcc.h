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


typedef struct Type Type;
typedef struct Node Node;



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
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize(char *input);


//
// parse.c
//

// local variable
typedef struct Obj Obj;
struct Obj
{
    Obj *next;
    char *name;     // variable name
    Type *ty;
    int offset;     
};

typedef struct Function Function;
struct Function
{
    Function *next;
    char *name;
    Obj *params;

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
    ND_ADDR,        // unary &
    ND_DEREF,       // unary *
    ND_RETURN,      // return
    ND_IF,          // if
    ND_FOR,         // for or while
    ND_BLOCK,       // {...}
    ND_FUNCALL,
    ND_EXPR_STMT,   // expression statement
    ND_VAR,         // variable
    ND_NUM
} NodeKind;


struct Node {
    NodeKind kind;  
    Token *tok;
    Type *ty;
    Node *next;
    Node *lhs;
    Node *rhs;

    // "if" or "for" statement
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;  

    // block
    Node *body;     

    // function name
    char *funcname;
    Node *args;

    Obj *var;       // used if kind == ND_VAR
    int val;        // used if kind == ND_NUM
};


Function *parse(Token *tok);


//
//  type.c
//
typedef enum {
    TY_INT,
    TY_PTR,
    TY_FUNC
} TypeKind;


struct Type
{
    TypeKind kind;

    // pointer
    Type *base;

    // declaration
    Token *name;

    // function type
    Type *return_ty;
    Type *params;
    Type *next;
};

extern Type *ty_int;

bool is_integer(Type *ty);
Type *copy_type(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
void add_type(Node *node);


//
// codegen.c
//

void codegen(Function *prog);


#endif /* PCC_H */
