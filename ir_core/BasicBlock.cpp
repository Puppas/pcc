#include <algorithm>
#include "Function.hpp"
#include "IRPrinter.hpp"


BB::BB(Function *parent, BB* before): 
    Value(ty_void, ValueKind::BB), parent(parent) 
{
    if (!before) 
        parent->get_bb_list().push_back(this);
    else
        parent->get_bb_list().insert(before, this);
}


BB::~BB()
{
    drop_all_references();
    for (auto iter = params.begin(); iter != params.end(); ++iter) {
        delete *iter;
    }
}

iterator_range<BrInst::succ_iterator> BB::successors() 
{
    if (BrInst* br_inst = dyn_cast<BrInst>(&back())) {
        return br_inst->successors();
    }
    else {
        return iterator_range<BrInst::succ_iterator>(
            BrInst::succ_iterator(back().op_end()), 
            BrInst::succ_iterator(back().op_end()));
    }
}


iterator_range<BrInst::const_succ_iterator> BB::successors() const 
{
    if (const BrInst* br_inst = dyn_cast<const BrInst>(&back())) {
        return br_inst->successors();
    }
    else {
        return iterator_range<BrInst::const_succ_iterator>(
            BrInst::const_succ_iterator(back().op_end()), 
            BrInst::const_succ_iterator(back().op_end()));
    }
}


BBParam* BB::insert_param(Type* ty) 
{
    BBParam* param = new BBParam(ty, this, params.size());
    params.push_back(param);
    return param;
}


BB::param_iterator BB::erase_param(param_list::size_type idx)
{
    delete *(params.begin() + idx);
    auto iter = params.erase(params.begin() + idx);
    for (; iter != params.end(); ++iter) {
        (*iter)->set_index(iter - params.begin());
    }

    return make_indirect_iterator(params.begin() + idx);
}


void BB::drop_all_references()
{
    for (auto &&inst : get_inst_list()) {
        inst.drop_all_references();
    }
}


void BB::insert_before(BB *pos)
{
    parent->get_bb_list().insert(pos, this);
}


void BB::insert_after(BB *pos)
{
    parent->get_bb_list().insert(std::next(Function::iterator(pos)), this);
}


ilist<BB>::iterator BB::erase_from_parent() {    
    return parent->get_bb_list().erase(this);
}


ilist<BB>::iterator BB::remove_from_parent()
{
    return parent->get_bb_list().remove(this);
}


ilist<BB>::iterator BB::move_before(BB *pos)
{
    auto next = remove_from_parent();
    insert_before(pos);
    return next;   
}

ilist<BB>::iterator BB::move_after(BB *pos)
{
    auto next = remove_from_parent();
    insert_after(pos);
    return next;
}


void BB::print(std::ostream& os, bool debug) const
{
    IRPrinter printer;
    printer.print(this, os, debug);
}

std::ostream& operator<<(std::ostream& os, const BB& bb)
{
    bb.print(os);
    return os;
}