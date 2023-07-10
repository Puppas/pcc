#ifndef PCC_TYPE_H
#define PCC_TYPE_H


struct Token;
struct Node;
struct Type;


struct Member
{
    Member *next;
    Type *ty;
    Token *name;
    int offset;
};


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

    Type() = default;

    Type(TypeKind kind, int size, int align) :
        kind(kind), size(size), align(align),
        base(nullptr), name(nullptr), array_len(0),
        members(nullptr), return_ty(nullptr),
        params(nullptr), next(nullptr) {}
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





#endif /* PCC_TYPE_H */
