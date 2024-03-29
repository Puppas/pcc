#include "Instruction.hpp"
#include "BasicBlock.hpp"
#include "Function.hpp"


Inst::Inst(Type* ty, ValueKind kind, BB* parent, Inst* before): 
    User(ty, kind), parent(parent) 
{
    if (parent)
        parent->get_inst_list().insert(before, this);
}


Inst* Inst::clone() const
{
    Inst* inst = new Inst(get_type(), get_kind(), nullptr, nullptr);
    for (int i = 0; i < this->get_num_operands(); ++i) {
        inst->add_operand(this->get_operand(i));
    }
    
    return inst;
}

void Inst::insert_before(Inst *pos)
{
    this->parent = pos->get_parent();
    parent->get_inst_list().insert(pos, this);
}


void Inst::insert_before(BB *bb, ilist<Inst>::iterator pos)
{
    this->parent = bb;
    parent->get_inst_list().insert(pos, this);
}


void Inst::insert_after(Inst *pos)
{
    this->parent = pos->get_parent();
    parent->get_inst_list().insert(std::next(BB::iterator(pos)), this);
}


BB::iterator Inst::erase_from_parent() 
{
    return parent->get_inst_list().erase(this);
}


BB::iterator Inst::remove_from_parent()
{
    return parent->get_inst_list().remove(this);
}


ilist<Inst>::iterator Inst::move_before(Inst *pos)
{
    auto next = remove_from_parent();
    insert_before(pos);
    return next;
}

ilist<Inst>::iterator Inst::move_before(BB *bb, ilist<Inst>::iterator pos)
{
    auto next = remove_from_parent();
    insert_before(bb, pos);
    return next;
}

ilist<Inst>::iterator Inst::move_after(Inst *pos)
{
    auto next = remove_from_parent();
    insert_after(pos);
    return next;
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


void BrInst::set_successor(op_size_type i, BB* bb)
{
    if (is_conditional()) {
        assert(i == 0 || i == 1);
        return set_operand(i + 1, bb);
    }
    else {
        assert(i == 0);
        return set_operand(i , bb);
    }
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

Inst *BrInst::clone() const
{
    BrInst* inst = cast<BrInst>(Inst::clone());
    inst->else_args_offset = this->else_args_offset;
    return inst;
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


void BrInst::remove_arg(op_size_type i, op_size_type idx)
{
    if (is_conditional()) {
        assert(i == 0 || i == 1);
        if (i == 0) {
            remove_operand(3 + idx);
            --else_args_offset;
        }
        else {
            remove_operand(else_args_offset + idx);
        } 
    }
    else {
        assert(i == 0);
        remove_operand(1 + idx);
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

