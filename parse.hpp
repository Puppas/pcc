#ifndef PCC_PARSE_H
#define PCC_PARSE_H


struct Token;
struct Member;
struct Type;
struct Node;

// variable or function
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
    bool is_static;

    // global variable
    char *init_data;

    // function
    Obj *params;
    Obj *locals;
    Node *body;
    int stack_size;
};


typedef enum
{
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_NEG,    // unary -
    ND_MOD,    // %
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // <
    ND_LE,     // <=
    ND_ASSIGN, // =
    ND_COMMA,  // ,
    ND_MEMBER, // . (struct member access)
    ND_ADDR,   // unary &
    ND_DEREF,  // unary *
    ND_NOT,    // !
    ND_LOGAND, // &&
    ND_LOGOR,  // ||
    ND_BITNOT, // ~
    ND_BITAND, // & 
    ND_BITOR,  // | 
    ND_BITXOR, // ^
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


#endif /* PCC_PARSE_H */
