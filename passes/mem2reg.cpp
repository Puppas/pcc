#include <assert.h>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include "mem2reg.hpp"
#include "ir_core/Module.hpp"
#include "utils/util.hpp"


static std::unordered_map<BB*, std::unordered_map<AllocaInst*, Value*>> m2r;
static std::unordered_map<Value*, Value*> r2r;

static std::unordered_map<BBParam*, AllocaInst*> param_to_var;
static std::unordered_map<BBParam*, std::vector<Value*>> param_to_args;
static std::vector<std::pair<BB*, BBParam*>> params_erased;

static std::unordered_set<BBParam*> visited;


static bool can_promote(const AllocaInst* ai)
{
    for (auto&& user: ai->get_users()) 
    {
        if (auto&& si = dyn_cast<const StoreInst>(&user)) {
            if (si->get_operand(0) == ai) {
                return false;
            }
        }
        else if (!isa<LoadInst>(&user)) {
            return false;
        }
    }

    return true;
}


static std::unordered_set<AllocaInst*> build_alloca_work_list(Function* fn)
{
    BB& entry = fn->front();
    std::unordered_set<AllocaInst*> alloca_work_list;

    for (auto&& inst = entry.begin(); inst != entry.end(); ++inst) {
        if (AllocaInst* ai = dyn_cast<AllocaInst>(to_address(inst))) {
            if (can_promote(ai)) {
                alloca_work_list.insert(ai);
            }
        }
    }

    return alloca_work_list;
}


static AllocaInst* in_work_list(Value* val, const std::unordered_set<AllocaInst*>& alloca_work_list)
{
    AllocaInst* ai = dyn_cast<AllocaInst>(val);
    if (!ai) 
        return nullptr;

    if (alloca_work_list.find(ai) == alloca_work_list.end())
        return nullptr;

    return ai;
}


static Value* find_val_trivial(AllocaInst* var, BB* block) 
{
    auto iter = m2r[block].find(var);
    if (iter != m2r[block].end()) {
        assert(iter->second);
        return iter->second;
    }

    if (block->get_pred_num() == 1) {
        m2r[block][var] = find_val_trivial(var, to_address(block->pred_begin()));
        return m2r[block][var];
    }

    BBParam* val = block->insert_param(var->get_type()->base);
    m2r[block][var] = val;
    param_to_var[val] = var;
    return val;
}


static void set_map(Function* fn, const std::unordered_set<AllocaInst*>& alloca_work_list) 
{
    for (auto bb = fn->begin(); bb != fn->end(); ++bb)
    {
        for (auto ir = bb->begin(); ir != bb->end(); ++ir)
        {
            if (ir->get_kind() == ValueKind::INST_STORE) {
                if (AllocaInst* ai = in_work_list(ir->get_operand(1), alloca_work_list))
                    m2r[to_address(bb)][ai] = ir->get_operand(0);
            }
            else if (ir->get_kind() == ValueKind::INST_LOAD) {
                if (AllocaInst* ai = in_work_list(ir->get_operand(0), alloca_work_list)) {
                    Value* val = find_val_trivial(ai, to_address(bb));
                    r2r[to_address(ir)] = val;
                }
            }
        }
    }
}


static Value* find_val(AllocaInst* var, BB* block);

std::vector<Value*> get_pred_vals(BBParam* param)
{
    BB* bb = param->get_parent();
    std::vector<Value*> record;
    std::set<Value*> vals;

    for (auto&& pred: bb->predecessors()) {
        Value* val = find_val(param_to_var[param], &pred);
        record.push_back(val);
        vals.insert(val);
    }

    if (vals.size() == 1) {
        return {record[0]};
    }
    else if (vals.size() == 2 && vals.find(param) != vals.end()) {
        vals.erase(param);
        return {*vals.begin()};
    }
    else {
        return record;
    }
}


static Value* set_arg(BBParam* param)
{
    if (visited.find(param) != visited.end())
        return param;

    BB* block = param->get_parent();
    visited.insert(param);

    std::vector<Value*> pred_vals = get_pred_vals(param);
    if (pred_vals.size() == 1) {
        Value* val = pred_vals[0];
        r2r[param] = val;
        m2r[block][param_to_var[param]] = val;
        assert(val != param);
        params_erased.emplace_back(block, param);
        return val;
    }

    param_to_args[param] = pred_vals;
    return param;
}


static Value* map_to(Value* val)
{
    Value* old = val;
    while (r2r[val])
        val = r2r[val];
    
    if (val != old)
        r2r[old] = val;
    return val;
}


static Value* find_val(AllocaInst* var, BB* block) 
{
    if (auto iter = m2r[block].find(var); iter != m2r[block].end()) {
        if (BBParam* param = dyn_cast<BBParam>(map_to(iter->second))) {
            set_arg(param);
        }
        return map_to(iter->second);
    }

    if (block->get_pred_num() == 1) {
        m2r[block][var] = find_val(var, to_address(block->pred_begin()));
        return m2r[block][var];
    }

    BBParam* val = block->insert_param(var->get_type()->base);
    m2r[block][var] = val;
    param_to_var[val] = var;
    assert(r2r.count(val) == 0);
    return set_arg(val);
}


static void set_args(Function* fn)
{
    for (auto bb = fn->begin(); bb != fn->end(); ++bb)
        for (auto iter = bb->param_begin(); iter != bb->param_end(); ++iter)
            set_arg(to_address(iter));
}


static void fill_args(Function* fn)
{
    for (auto bb = fn->begin(); bb != fn->end(); ++bb)
    {
        for (auto param = bb->param_begin(); param != bb->param_end(); ++param)
        {
            int i = 0;
            std::vector<Value*>& args = param_to_args[to_address(param)];
            for (auto pred = bb->pred_begin(); pred != bb->pred_end(); ++pred)
            {
                BrInst& last = cast<BrInst>(pred->back());
                if (last.is_unconditional()) 
                    last.add_arg(0, args[i]);
                else if (last.get_successor(0) == to_address(bb)) 
                    last.add_arg(0, args[i]);
                else
                    last.add_arg(1, args[i]);

                ++i;
            }
        }
    }
}


static void add_bb_args(Function* fn, const std::unordered_set<AllocaInst*>& work_list) 
{
    set_map(fn, work_list);
    set_args(fn);

    for (auto&& p: params_erased) {
        p.first->erase_param(p.second->get_index());
    }

    fill_args(fn);
}


static void rewrite(Function* fn, const std::unordered_set<AllocaInst*>& work_list)
{
    for (auto ai: work_list)
    {
        for (auto&& user = ai->user_begin(); user != ai->user_end(); )
        {
            if (StoreInst* si = dyn_cast<StoreInst>(to_address(user))) {
                ++user;
                si->erase_from_parent();
            }  
            else if (LoadInst* li = dyn_cast<LoadInst>(to_address(user))) {
                ++user;
                li->replace_all_uses_with(map_to(li));
                li->erase_from_parent();
            }
            else {
                unreachable();
            } 
        }

        ai->erase_from_parent();
    }
}


void mem2reg(Module* module)
{
    for (auto fn = module->begin(); fn != module->end(); ++fn) 
    {        
        m2r.clear();
        r2r.clear();
        visited.clear();
        param_to_var.clear();
        param_to_args.clear();
        params_erased.clear();
        
        std::unordered_set<AllocaInst*> work_list = build_alloca_work_list(to_address(fn));
        add_bb_args(to_address(fn), work_list);  
        rewrite(to_address(fn), work_list);       
    }
}





