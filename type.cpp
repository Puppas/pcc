#include <stdlib.h>
#include <string>
#include "type.hpp"
#include "parse.hpp"
#include "tokenize.hpp"



Type *ty_void = new Type(TY_VOID, 1, 1);
Type *ty_bool = new Type(TY_BOOL, 1, 1);
Type *ty_char = new Type(TY_CHAR, 1, 1);
Type *ty_short = new Type(TY_SHORT, 2, 2);
Type *ty_int = new Type(TY_INT, 4, 4);
Type *ty_long = new Type(TY_LONG, 8, 8);


static Type *new_type(TypeKind kind, int size, int align) {
    Type *ty = (Type*)calloc(1, sizeof(Type));
    ty->kind = kind;
    ty->size = size;
    ty->align = align;
    return ty;
}


bool is_integer(Type *ty) {
    TypeKind k = ty->kind;
    return k == TY_BOOL || k == TY_CHAR || k == TY_SHORT ||
           k == TY_INT || k == TY_LONG || k == TY_ENUM;
}

    
Type *copy_type(Type *ty) {
    Type *ret = (Type*)calloc(1, sizeof(Type));
    *ret = *ty;
    return ret;
}

Type *pointer_to(Type *base) {
    Type *ty = new_type(TY_PTR, 8, 8);
    ty->base = base;
    return ty;
}


Type *func_type(Type *return_ty) {
    Type *ty = (Type*)calloc(1, sizeof(Type));
    ty->kind = TY_FUNC;
    ty->return_ty = return_ty;
    return ty;
}


Type *array_of(Type *base, int len) {
    Type *ty = new_type(TY_ARRAY, base->size * len, base->align);
    ty->base = base;
    ty->array_len = len;
    return ty;
}

Type *enum_type() {
    return new_type(TY_ENUM, 4, 4);
}


static Type *get_common_type(Type *ty1, Type *ty2) {
    if(ty1->base)
        return pointer_to(ty1->base);
    if(ty1->size == 8 || ty2->size == 8)
        return ty_long;
    
    return ty_int;
}


// for arithmetic operators, we implicitly promote operands so that
// both operands have the same type. Any intergral type should be promoted
// to int if it's smaller then int. If the type of one operand is larger
// than the other's (e.g. "long" vs. "int"), the smaller operand will
// be promoted to match with the other.
static void usual_arith_conversion(Node **lhs, Node **rhs) {
    Type *ty = get_common_type((*lhs)->ty, (*rhs)->ty);

    if ((*lhs)->ty->kind != ty->kind)
        *lhs = new_cast(*lhs, ty);
    
    if ((*rhs)->ty->kind != ty->kind)
        *rhs = new_cast(*rhs, ty);
}


void add_type(Node *node) {
    if (!node || node->ty) {
        return;
    }

    add_type(node->lhs);
    add_type(node->rhs);
    add_type(node->cond);
    add_type(node->then);
    add_type(node->els);
    add_type(node->init);
    add_type(node->inc);

    for (Node *n = node->body; n; n = n->next) {
        add_type(n);
    }

    for (Node *n = node->args; n; n = n->next) {
        add_type(n);
    }

    switch (node->kind)
    {
    case ND_NUM:
        node->ty = (node->val == (int)node->val) ? ty_int : ty_long;
        return;
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_MOD:
    case ND_BITAND:
    case ND_BITOR:
    case ND_BITXOR:
        usual_arith_conversion(&node->lhs, &node->rhs);
        node->ty = node->lhs->ty;
        return;
    case ND_NEG: {
        Type *ty = get_common_type(ty_int, node->lhs->ty);
        if (node->lhs->ty->kind != ty->kind)
            node->lhs = new_cast(node->lhs, ty);
        node->ty = ty;
        return;
    }
    case ND_ASSIGN:
        if(node->lhs->ty->kind == TY_ARRAY) {
            error_tok(node->lhs->tok, "not an lvalue");
        }
        if (node->lhs->ty->kind != TY_STRUCT && node->lhs->ty->kind != node->rhs->ty->kind) {
            node->rhs = new_cast(node->rhs, node->lhs->ty);
        }
        node->ty = node->lhs->ty;
        return;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
        usual_arith_conversion(&node->lhs, &node->rhs);
        node->ty = ty_int;
        return;
    case ND_FUNCALL:
        node->ty = ty_long;
        return;
    case ND_NOT:
    case ND_LOGAND:
    case ND_LOGOR:
        node->ty = ty_int;
        return;
    case ND_BITNOT:
        node->ty = node->lhs->ty;
        return;
    case ND_VAR:
        node->ty = node->var->ty;
        return;
    case ND_COMMA:
        node->ty = node->rhs->ty;
        return;
    case ND_MEMBER:
        node->ty = node->member->ty;
        return;
    case ND_ADDR:
        if (node->lhs->ty->kind == TY_ARRAY) {
            node->ty = pointer_to(node->lhs->ty->base);
        }
        else {
            node->ty = pointer_to(node->lhs->ty);
        }
        return;
    case ND_DEREF: {
        if (!node->lhs->ty->base) 
            error_tok(node->tok, "invalid pointer dereference");
        if (node->lhs->ty->base->kind == TY_VOID)
            error_tok(node->tok, "dereferencing a void pointer");

        node->ty = node->lhs->ty->base;
        return;
    }
    case ND_STMT_EXPR:
        if (node->body) {
            Node *stmt = node->body;
            while (stmt->next)
                stmt = stmt->next;
            if (stmt->kind == ND_EXPR_STMT) {
                // set for the last expression type
                node->ty = stmt->lhs->ty;
                return;
            }
        }
        error_tok(node->tok, "statement expression returning void is not supported");
        return;
    }
}

