#include <assert.h>
#include <unordered_map>
#include <algorithm>
#include <set>
#include "gen_ir.hpp"
#include "parse.hpp"
#include "type.hpp"
#include "tokenize.hpp"
#include "utils/util.hpp"
#include "ir_core/IRContext.hpp"
#include "ir_core/IRBuilder.hpp"


static std::unordered_map<Obj*, Value*> alloca_map;
static std::set<BB*> ret_blocks;


static Value *gen_expr(Node *node, IRBuilder& builder);


// In C, all expressions that can be written on the left-hand side of
// the '=' operator must have an address in memory. In other words, if
// you can apply the '&' operator to take an address of some
// expression E, you can assign E to a new value.
//
// Other expressions, such as `1+2`, cannot be written on the lhs of
// '=', since they are just temporary values that don't have an address.
//
// The stuff that can be written on the lhs of '=' is called lvalue.
// Other values are called rvalue. An lvalue is essentially an address.
//
// When lvalues appear on the rvalue context, they are converted to
// rvalues by loading their values from their addresses. You can think
// '&' as an operator that suppresses such automatic lvalue-to-rvalue
// conversion.
//
// This function evaluates a given node as an lvalue.
static Value *gen_addr(Node *node, IRBuilder& builder) {
  switch (node->kind)
  {
  case ND_VAR:
    return alloca_map[node->var];
  case ND_DEREF:
    return gen_expr(node->lhs, builder);
  case ND_MEMBER: {
    return builder.create_binary(ValueKind::INST_ADD, 
      gen_addr(node->lhs, builder), 
      builder.get_int(node->member->offset));
  }
  case ND_COMMA: 
    gen_expr(node->lhs, builder);
    return gen_addr(node->rhs, builder);
  }

  error_tok(node->tok, "not an lvalue");
}


static Value *gen_binop(ValueKind kind, Node *node, IRBuilder& builder) {
  return builder.create_binary(kind, 
    gen_expr(node->lhs, builder), 
    gen_expr(node->rhs, builder));
}


static void gen_stmt(Node *node, Function* function, IRBuilder& builder);

static Value *gen_expr(Node *node, IRBuilder& builder) {
  switch (node->kind) {
  case ND_NUM:
    return builder.get_int(node->val);
  case ND_NEG: {
    Value *val = gen_expr(node->lhs, builder);
    return builder.create_unary(ValueKind::INST_NEG, val);
  }
  case ND_VAR:
  case ND_MEMBER: {
    return builder.create_load(gen_addr(node, builder));
  }
  case ND_LOGAND: {
    Function* function = builder.get_insert_block()->get_parent();
    BB *bb = BB::create(function);
    BB *set0 = BB::create(function);
    BB *set1 = BB::create(function);
    BB *last = BB::create(function);

    builder.create_cond_br(gen_expr(node->lhs, builder), bb, set0);

    builder.set_insert_point(bb);
    builder.create_cond_br(gen_expr(node->rhs, builder), set1, set0);

    builder.set_insert_point(set0);
    builder.create_br(last, {builder.get_int(0)});

    builder.set_insert_point(set1);
    builder.create_br(last, {builder.get_int(1)});

    builder.set_insert_point(last);
    return last->insert_param(ty_int);
  }
  case ND_LOGOR: {
    Function* function = builder.get_insert_block()->get_parent();
    BB *bb = BB::create(function);
    BB *set0 = BB::create(function);
    BB *set1 = BB::create(function);
    BB *last = BB::create(function);

    builder.create_cond_br(gen_expr(node->lhs, builder), set1, bb);

    builder.set_insert_point(bb);
    builder.create_cond_br(gen_expr(node->rhs, builder), set1, set0);

    builder.set_insert_point(set1);
    builder.create_br(last, {builder.get_int(1)});

    builder.set_insert_point(set0);
    builder.create_br(last, {builder.get_int(0)});

    builder.set_insert_point(last);    
    return last->insert_param(ty_int);
  }
  case ND_FUNCALL: {
    Module *module = builder.get_insert_block()->get_parent()->get_parent();
    std::vector<Value*> args;
    
    for (Node* arg = node->args; arg; arg = arg->next) {
      args.push_back(gen_expr(arg, builder));
    }

    return builder.create_call(module->get_function(node->funcname), args);
  }
  case ND_ADDR:
    return gen_addr(node->lhs, builder);
  case ND_DEREF: {
    return builder.create_load(gen_expr(node->lhs, builder));
  }
  case ND_CAST: {
    return builder.create_cast(node->ty, gen_expr(node->lhs, builder));
  }
  case ND_STMT_EXPR: {
    Function* function = builder.get_insert_block()->get_parent();
    Node *n = node->body;
    for (; n && n->next; n = n->next)
      gen_stmt(n, function, builder);
    return gen_expr(n, builder);
  }
  case ND_ASSIGN: {
    Value *val = gen_expr(node->rhs, builder);
    Value *addr = gen_addr(node->lhs, builder);
    builder.create_store(val, addr);
    return val;
  }
  case ND_ADD:
    return gen_binop(ValueKind::INST_ADD, node, builder);
  case ND_SUB:
    return gen_binop(ValueKind::INST_SUB, node, builder);
  case ND_MUL:
    return gen_binop(ValueKind::INST_MUL, node, builder);
  case ND_DIV:
    return gen_binop(ValueKind::INST_DIV, node, builder);
  case ND_MOD:
    return gen_binop(ValueKind::INST_MOD, node, builder);
  case ND_EQ:
    return gen_binop(ValueKind::INST_EQ, node, builder);
  case ND_NE:
    return gen_binop(ValueKind::INST_NE, node, builder);
  case ND_LT:
    return gen_binop(ValueKind::INST_LT, node, builder);
  case ND_LE:
    return gen_binop(ValueKind::INST_LE, node, builder);
  case ND_BITAND:
    return gen_binop(ValueKind::INST_BITAND, node, builder);
  case ND_BITOR:
    return gen_binop(ValueKind::INST_BITOR, node, builder);
  case ND_BITXOR:
    return gen_binop(ValueKind::INST_BITXOR, node, builder);
  case ND_BITNOT: {
    return builder.create_unary(ValueKind::INST_BITNOT, gen_expr(node->lhs, builder));
  }
  case ND_COMMA:
    gen_expr(node->lhs, builder);
    return gen_expr(node->rhs, builder);
  case ND_NOT: {
    return builder.create_binary(ValueKind::INST_EQ, 
      gen_expr(node->lhs, builder), 
      builder.get_int(0));
  }
  }

  error_tok(node->tok, "invalid expression");
}

static void gen_stmt(Node *node, Function* function, IRBuilder& builder) {
  switch (node->kind) {
  case ND_IF: {
    BB *then = BB::create(function);
    BB *els = BB::create(function);
    BB *last = BB::create(function);

    builder.create_cond_br(gen_expr(node->cond, builder), then, els);
    builder.set_insert_point(then);

    gen_stmt(node->then, function, builder);

    if (!builder.get_insert_block()->back().is_terminator())
      builder.create_br(last);


    builder.set_insert_point(els);
    if (node->els)
      gen_stmt(node->els, function, builder);
    if (!builder.get_insert_block()->back().is_terminator())
      builder.create_br(last);

    builder.set_insert_point(last);
    return;
  }
  case ND_FOR: {
    BB* cond = BB::create(function);
    BB* body = BB::create(function);;
    BB* inc = BB::create(function);;
    BB* last = BB::create(function);;
    
    if (node->init)
      gen_stmt(node->init, function, builder);

    builder.create_br(cond);    
    builder.set_insert_point(cond);
    
    if (node->cond)
      builder.create_cond_br(gen_expr(node->cond, builder), body, last);
    else
      builder.create_br(body);
    
    builder.set_insert_point(body);
    gen_stmt(node->then, function, builder);
    if (!builder.get_insert_block()->back().is_terminator())
      builder.create_br(inc);

    builder.set_insert_point(inc);
    if (node->inc)
      gen_expr(node->inc, builder);
    builder.create_br(cond);
    
    builder.set_insert_point(last);
    return;
  }
  case ND_RETURN: {
    builder.create_ret(gen_expr(node->lhs, builder));
    ret_blocks.insert(builder.get_insert_block());
    return;
  }
  case ND_EXPR_STMT:
    gen_expr(node->lhs, builder);
    return;
  case ND_BLOCK:
    for (Node *n = node->body; n; n = n->next)
        gen_stmt(n, function, builder);
    return;
  }

  error_tok(node->tok, "invalid statement");
}


// Assign offsets to local variables.
static void assign_lvar_offsets(Obj *prog)
{
    for (Obj *fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function)
            continue;

        int offset = 0;
        for (Obj *var = fn->locals; var; var = var->next)
        {
            offset += var->ty->size;
            offset = align_to(offset, var->ty->align);
            var->offset = -offset;
        }
        fn->stack_size = align_to(offset, 16);
    }
}



static void gen_gvar_ir(Obj* prog, Module* module)
{
    for (Obj *var = prog; var; var = var->next)
    {
        if (var->is_function)
            continue;

        GlobalVariable* gvar = module->get_or_insert_global(var->ty, var->name);
        alloca_map[var] = gvar;
    }
}


static void gen_alloca_ir(Obj* fn, IRBuilder& builder)
{
    for (Obj *var = fn->locals; var; var = var->next)
        alloca_map[var] = builder.create_alloca(var->ty);
}

static void store_param(Obj *fn, Function* function, IRBuilder& builder) 
{
    auto iter = function->param_begin();
    for (Obj* var = fn->params; var; var = var->next, ++iter)
    {
        builder.create_store(to_address(iter), alloca_map[var]);
    }
}


void unify_return_blocks(Function *f, IRBuilder& builder) 
{
    BB* entry = &f->front();
    builder.set_insert_point(to_address(entry->begin()));
    AllocaInst* new_ret_alloca = builder.create_alloca(f->get_return_type());

    BB *new_ret_block = BB::create(f);
    builder.set_insert_point(new_ret_block);
    builder.create_ret(builder.create_load(new_ret_alloca));

    for (BB* bb: ret_blocks)
    {
        RetInst* ret_inst = cast<RetInst>(&bb->back());
        builder.set_insert_point(bb);
        builder.create_store(ret_inst->get_operand(0), new_ret_alloca);
        builder.create_br(new_ret_block);
        ret_inst->erase_from_parent();
    }
}


static void gen_func_ir(Obj *prog, Module* module, IRContext& context)
{
    for (Obj* fn = prog; fn; fn = fn->next)
    {
        if (!fn->is_function || !fn->is_definition)
          continue;

        Function *function = module->get_or_insert_funtion(fn->ty, fn->name);
        BB* entry = BB::create(function);
        IRBuilder builder(context, entry);

        gen_alloca_ir(fn, builder);
        store_param(fn, function, builder);

        gen_stmt(fn->body, function, builder);

        if (ret_blocks.size() > 1) {
            unify_return_blocks(function, builder);
        }
        else if (*ret_blocks.begin() != &function->back()) {
            BB* ret_bb = *ret_blocks.begin();
            ret_bb->move_after(&function->back());
        }

        ret_blocks.clear();
    }
}


Module* gen_ir(Obj *prog, IRContext& context) {
  assign_lvar_offsets(prog);

  Module* module = new Module(context);
  gen_gvar_ir(prog, module);
  gen_func_ir(prog, module, context);
  return module;
}
