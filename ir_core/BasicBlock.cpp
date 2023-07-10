#include <algorithm>
#include "Function.hpp"


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


Value* BB::insert_param(Type* ty) 
{
    Value* param = new Value(ty, ValueKind::VALUE);
    params.push_back(param);
    return param;
}


void BB::erase_param(Value* val)
{
    auto iter = std::find(params.begin(), params.end(), val);
    params.erase(iter);
    delete val;
}


void BB::drop_all_references()
{
    for (auto &&inst : get_inst_list()) {
        inst.drop_all_references();
    }
}

ilist<BB>::iterator BB::erase_from_parent() {    
    return parent->get_bb_list().erase(this);
}
