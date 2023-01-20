#ifndef PCC_H
#define PCC_H

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;
typedef struct Node Node;
typedef struct Member Member;

//
// strings.c
//

char *format(char *fmt, ...);

//
// tokenize.c
//

typedef enum
{
    TK_PUNCT,   // Punctuators
    TK_IDENT,   // Identifiers
    TK_KEYWORD, // Keywords
    TK_STR,     // String literals
    TK_NUM,
    TK_EOF
} TokenKind;

typedef struct Token Token;

struct Token
{
    TokenKind kind;
    Token *next;
    int64_t val; // used if kind is TK_NUM
    char *loc;
    int len;
    Type *ty;    // used if TK_STR
    char *str;   // string literal contents
    int line_no; // line number
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize_file(char *filename);

#define unreachable() \
    error("internal error at %s:%d", __FILE__, __LINE__)

//
// parse.c
//

// variable or function
typedef struct Obj Obj;
struct Obj
{
    Obj *next;
    char *name;
    Type *ty;
    bool is_local; // local or global

    // local variable
    int offset;

    bool is_function;
    bool is_definition;

    // global variable
    char *init_data;

    // function
    Obj *params;
    Node *body;
    Obj *locals;
    int stack_size;
};

typedef enum
{
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_NEG,    // unary -
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // <
    ND_LE,     // <=
    ND_ASSIGN, // =
    ND_COMMA,  // ,
    ND_MEMBER, // . (struct member access)
    ND_ADDR,   // unary &
    ND_DEREF,  // unary *
    ND_RETURN, // return
    ND_IF,     // if
    ND_FOR,    // for or while
    ND_BLOCK,  // {...}
    ND_FUNCALL,
    ND_EXPR_STMT, // expression statement
    ND_STMT_EXPR, // statement expression
    ND_VAR,       // variable
    ND_NUM,
    ND_CAST // type cast
} NodeKind;

struct Node
{
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

    // block or statement expression
    Node *body;

    // struct member access
    Member *member;

    // function call
    char *funcname;
    Type *func_ty;
    Node *args;

    Obj *var;    // used if kind == ND_VAR
    int64_t val; // used if kind == ND_NUM
};


Node *new_cast(Node *expr, Type *ty);
Obj *parse(Token *tok);

//
//  type.c
//
typedef enum
{
    TY_VOID,
    TY_BOOL,
    TY_CHAR,
    TY_SHORT,
    TY_INT,
    TY_LONG,    
    TY_ENUM,
    TY_PTR,
    TY_FUNC,
    TY_ARRAY,
    TY_STRUCT,
    TY_UNION
} TypeKind;

struct Type
{
    TypeKind kind;
    int size; // returned by sizeof()
    int align;

    // pointer or array
    Type *base;

    // declaration
    Token *name;

    int array_len;

    // struct
    Member *members;

    // function type
    Type *return_ty;
    Type *params;
    Type *next;
};

struct Member
{
    Member *next;
    Type *ty;
    Token *name;
    int offset;
};

extern Type *ty_void;
extern Type *ty_bool;
extern Type *ty_char;
extern Type *ty_int;
extern Type *ty_long;
extern Type *ty_short;


bool is_integer(Type *ty);
Type *copy_type(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
Type *array_of(Type *base, int size);
Type *enum_type();
void add_type(Node *node);

//
// codegen.c
//

void codegen(Obj *prog, FILE *out);
int align_to(int n, int align);

#endif /* PCC_H */
