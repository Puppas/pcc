#include "Instruction.hpp"
#include "BasicBlock.hpp"
#include "Function.hpp"


Inst::Inst(Type* ty, ValueKind kind, BB* parent, Inst* before): 
    User(ty, kind), parent(parent) 
{
    parent->get_inst_list().insert(before, this);
}


ilist<Inst>::iterator Inst::erase_from_parent() 
{
    return parent->get_inst_list().erase(this);
}


UnaryInst::UnaryInst(ValueKind kind, Value* src, BB* parent, Inst* before): 
    Inst(src->get_type(), kind, parent, before)
{
    add_operand(src);
}


BinaryInst::BinaryInst(ValueKind kind, Value* lhs, Value* rhs, BB* parent, Inst* before): 
    Inst(lhs->get_type(), kind, parent, before)
{
    add_operand(lhs);
    add_operand(rhs);
}


RetInst::RetInst(Value* ret, BB* parent, Inst* before): 
    Inst(ty_void, ValueKind::INST_RETURN, parent, before) 
{
    add_operand(ret);
}


StoreInst::StoreInst(Value* src, Value* dst, BB* parent, Inst* before): 
    Inst(ty_void, ValueKind::INST_STORE, parent, before)
{
    add_operand(src);
    add_operand(dst);
}



BrInst::BrInst(BB* then_, BB* parent, Inst* before, const std::vector<Value*>& then_args):
    Inst(ty_void, ValueKind::INST_BR, parent, before),
    else_args_offset(-1)
{
    add_operand(then_);
    for (auto &&arg : then_args)
        add_operand(arg);
}


BrInst::BrInst(Value* cond, BB* then_, BB* else_, BB* parent, Inst* before,
    const std::vector<Value*>& then_args, 
    const std::vector<Value*>& else_args):
    Inst(ty_void, ValueKind::INST_BR, parent, before),
    else_args_offset(3 + then_args.size())
{
    add_operand(cond);
    add_operand(then_);
    add_operand(else_);
 
    for (auto &&arg : then_args)
        add_operand(arg);
    for (auto &&arg : else_args)
        add_operand(arg);
}


const BB* BrInst::get_successor(op_size_type i) const { 
    if (is_conditional()) {
        assert(i == 0 || i == 1);
        return cast<BB>(get_operand(i + 1).get());
    }
    else {
        assert(i == 0);
        return cast<BB>(get_operand(0).get()); 
    }
}


BB* BrInst::get_successor(op_size_type i) {
    return const_cast<BB*>(static_cast<const BrInst*>(this)->get_successor(i));
}

iterator_range<BrInst::succ_iterator> 
BrInst::successors() {
    if (is_conditional())
        return make_range(succ_iterator(op_begin() + 1), succ_iterator(op_begin() + 3));
    else 
        return make_range(succ_iterator(op_begin()), succ_iterator(op_begin() + 1));
}

iterator_range<BrInst::const_succ_iterator> 
BrInst::successors() const {
    if (is_conditional())
        return make_range(const_succ_iterator(op_begin() + 1), const_succ_iterator(op_begin() + 3));
    else 
        return make_range(const_succ_iterator(op_begin()), const_succ_iterator(op_begin() + 1));
}


BrInst::op_size_type BrInst::get_num_args(op_size_type i) const {
    assert(i == 0 || i == 1);

    if (is_conditional()) {
        return i == 0 ? else_args_offset - 3 : get_num_operands() - else_args_offset;
    }
    else {
        assert(i == 0);
        return get_num_operands() - 1;
    }
}


void BrInst::add_arg(op_size_type i, Value *arg) {
    if (is_conditional()) {
        assert(i == 0 || i == 1);
        if (i == 0) {
            add_operand(op_begin() + else_args_offset, arg);
            ++else_args_offset;
        }
        else {
            add_operand(arg);
        } 
    }
    else {
        assert(i == 0);
        add_operand(arg);
    }
}


void BrInst::add_arg(op_size_type i, Value *arg, op_size_type loc) {
    if (is_conditional()) {
        assert(i == 0 || i == 1);
        if (i == 0) {
            add_operand(op_begin() + 3 + loc, arg);
            ++else_args_offset;
        }
        else {
            add_operand(op_begin() + else_args_offset + loc, arg);
        } 
    }
    else {
        assert(i == 0);
        add_operand(op_begin() + 1 + loc, arg);
    }
}


iterator_range<BrInst::op_iterator> 
BrInst::get_args(op_size_type i) 
{
    assert(i == 0 || i == 1);
    
    if (i == 0) {
        if (is_conditional())
            return make_range(op_begin() + 3, op_begin() + else_args_offset);
        else 
            return make_range(op_begin() + 1, op_end());
    }
    else {
        assert(is_conditional());
        return make_range(op_begin() + else_args_offset, op_end());
    }
}

iterator_range<BrInst::const_op_iterator> 
BrInst::get_args(op_size_type i) const
{
    assert(i == 0 || i == 1);
    
    if (i == 0) {
        if (is_conditional())
            return make_range(op_begin() + 3, op_begin() + else_args_offset);
        else 
            return make_range(op_begin() + 1, op_end());
    }
    else {
        assert(is_conditional());
        return make_range(op_begin() + else_args_offset, op_end());
    }
}


CallInst::CallInst(Function* callee, const std::vector<Value*>& args, BB* parent, Inst* before):
    Inst(callee->get_return_type(), ValueKind::INST_CALL, parent, before)
{
    add_operand(callee);
    for (auto &&arg : args)
        add_operand(arg);        
}

