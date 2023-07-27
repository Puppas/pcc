#include <queue>
#include "ir_core/Function.hpp"
#include "ir_core/POTraversal.hpp"
#include "ir_core/IRBuilder.hpp"
#include "ir_core/Dominators.hpp"


static std::unordered_map<BB*, std::vector<BB*>> 
calculate_rdf(Function* fn, const PostDominatorTree& tree)
{
    using GT = InverseGraphTraits<Function>;

    std::unordered_map<BB*, std::vector<BB*>> rdf;
    for (auto bb = fn->begin(); bb != fn->end(); ++bb)
        rdf[to_address(bb)] = {};

    for (auto bb = fn->begin(); bb != fn->end(); ++bb)
    {
        BB* bb_ptr = to_address(bb);
        if (GT::parent_end(bb_ptr) - GT::parent_begin(bb_ptr) < 2)
            continue;
        
        for (auto pred = GT::parent_begin(bb_ptr); pred != GT::parent_end(bb_ptr); ++pred)
        {
            BB* runner = to_address(pred);
            while (runner != tree.get_node(bb_ptr)->get_idom()->get_block() && runner != bb_ptr)
            {
                rdf[runner].push_back(bb_ptr);
                runner = tree.get_node(runner)->get_idom()->get_block();
            }
        }
    }

    return rdf;
}



static bool is_critical(Inst* inst)
{
    if (dyn_cast<RetInst>(inst))
        return true;
    
    if (StoreInst* store_inst = dyn_cast<StoreInst>(inst)) {
        if(dyn_cast<GlobalObject>(store_inst->get_operand(1).get())) 
            return true;
    }

    if (BrInst* br = dyn_cast<BrInst>(inst); br && br->is_unconditional())
        return true;

    return false;
}


static void add_to_work_list(Value* val, std::unordered_set<Value*>& marked, std::queue<Value*>& work_list)
{
    if (marked.find(val) == marked.end()) {
        marked.insert(val);
        work_list.push(val);
    }
}


static void mark(Value* val, std::unordered_set<Value*>& marked, 
    std::queue<Value*>& work_list, const std::unordered_map<BB*, std::vector<BB*>>& rdf,
    std::unordered_set<BB*>& useful_block)
{
    if (BinaryInst* binary_inst = dyn_cast<BinaryInst>(val)) {
        add_to_work_list(binary_inst->get_operand(0), marked, work_list);
        add_to_work_list(binary_inst->get_operand(1), marked, work_list);
    }
    else if (UnaryInst* unary_inst = dyn_cast<UnaryInst>(val)) {
        add_to_work_list(unary_inst->get_operand(0), marked, work_list);
    }
    else if (RetInst* ret_inst = dyn_cast<RetInst>(val)) {
        add_to_work_list(ret_inst->get_operand(0), marked, work_list);
    }
    else if (BrInst* br_inst = dyn_cast<BrInst>(val)) {
        if (br_inst->is_conditional()) 
            add_to_work_list(br_inst->get_condition(), marked, work_list);
    }
    else if (StoreInst* store_inst = dyn_cast<StoreInst>(val)) {
        add_to_work_list(store_inst->get_operand(0), marked, work_list);
        add_to_work_list(store_inst->get_operand(1), marked, work_list);
    }
    else if (CallInst* call_inst = dyn_cast<CallInst>(val)) {
        for (auto&& arg: call_inst->args()) {
            add_to_work_list(arg.get(), marked, work_list);
        }
    }
    else if (BBParam* param = dyn_cast<BBParam>(val)) {
        std::size_t index = param->get_index();
        for (auto&& pred: param->get_parent()->predecessors()) {
            BrInst& br_inst = cast<BrInst>(pred.back());
            add_to_work_list(&br_inst, marked, work_list);

            if (br_inst.get_successor(0) == param->get_parent())
                add_to_work_list(br_inst.get_args(0)[index], marked, work_list);
            else
                add_to_work_list(br_inst.get_args(1)[index], marked, work_list);
        }
    }

    BB* bb = nullptr;
    if (Inst* inst = dyn_cast<Inst>(val)) {
        bb = inst->get_parent();
    }
    else if (BBParam* p = dyn_cast<BBParam>(val)) {
        bb = p->get_parent();
    }

    if (bb) {
        for (BB* frontier: rdf.find(bb)->second) 
            add_to_work_list(&frontier->back(), marked, work_list);

        useful_block.insert(bb);
    }
}


static std::pair<std::unordered_set<Value*>, std::unordered_set<BB*>> 
mark(Function* fn, const PostDominatorTree& tree)
{
    std::unordered_set<Value*> marked;
    std::unordered_set<BB*> useful_block;
    std::queue<Value*> work_list;

    for (auto bb = fn->begin(); bb != fn->end(); ++bb) {
        for (auto inst = bb->begin(); inst != bb->end(); ++inst) {
            if (is_critical(to_address(inst))) {
                marked.insert(to_address(inst));
                work_list.push(to_address(inst));
            }
        }
    }

    std::unordered_map<BB*, std::vector<BB*>> rdf = calculate_rdf(fn, tree);
    while (!work_list.empty()) {
        mark(work_list.front(), marked, work_list, rdf, useful_block);
        work_list.pop();
    }

    return std::make_pair(marked, useful_block);
}


static BB* find_marked_postdominator(BB* bb, 
    const std::unordered_set<BB*>& useful_block, const PostDominatorTree& tree)
{
    auto target = tree.get_node(bb)->get_idom();
    while (useful_block.find(target->get_block()) == useful_block.end()) {
        target = target->get_idom();
    }

    return target->get_block();
}


static void sweep(Function* fn, const std::unordered_set<Value*>& marked, 
    const std::unordered_set<BB*>& useful_block, const PostDominatorTree& tree)
{
    for (auto bb = fn->begin(); bb != fn->end(); ++bb) 
    {
        for (auto param = bb->param_begin(); param != bb->param_end(); ) 
        {
            if (marked.find(to_address(param)) == marked.end()) {
                for (auto&& pred: bb->predecessors()) {
                    BrInst& br_inst = cast<BrInst>(pred.back());
                    if (br_inst.get_successor(0) == to_address(bb)) {
                        br_inst.remove_arg(0, param->get_index());
                    }
                    else {
                        br_inst.remove_arg(1, param->get_index());
                    }
                }
                
                param->replace_all_uses_with(nullptr);
                param = bb->erase_param(param->get_index());
            }
            else {
                ++param;
            }
        }

        for (auto inst = bb->begin(); inst != bb->end(); ) 
        {
            if (marked.find(to_address(inst)) == marked.end()) {
                if (BrInst* br = dyn_cast<BrInst>(to_address(inst)); br && br->is_conditional()) {
                    BB* target = find_marked_postdominator(to_address(bb), useful_block, tree);
                    assert(target->param_size() == 0);
                    IRBuilder builder(fn->get_context(), to_address(bb));
                    builder.create_br(target);
                    br->erase_from_parent();
                    break;
                }
                else {
                    inst->replace_all_uses_with(nullptr);
                    inst = inst->erase_from_parent();
                }
            }
            else {
                ++inst;
            }
        }
    }
}


static bool is_redundant_cond_br(BrInst* br)
{
    if (br->get_successor(0) != br->get_successor(1))
        return false;

    if (br->get_num_args(0) != br->get_num_args(1))
        return false;

    for (int i = 0; i < br->get_num_args(0); ++i)
        if (br->get_args(0)[i] != br->get_args(1)[i])
            return false;

    return true;
}


static bool just_forwarding(BB* bb)
{
    BrInst* br = cast<BrInst>(&bb->back());
    if (br->is_unconditional())
    {
        if (bb->param_size() != br->get_args(0).size())
            return false;

        int n = bb->param_size();
        auto params = bb->get_params();
        auto args = br->get_args(0);

        for (int i = 0; i < n; ++i) {
            if (dyn_cast<BBParam>(args[i].get()) != &params[i]) 
                return false;
        }
    }
    else
    {
        int n = bb->param_size();
        if (n != br->get_args(0).size() || n != br->get_args(1).size())
            return false;

        auto params = bb->get_params();
        auto args1 = br->get_args(0);
        auto args2 = br->get_args(1);

        for (int i = 0; i < n; ++i) {
            if (dyn_cast<BBParam>(args1[i].get()) != &params[i] || 
                dyn_cast<BBParam>(args2[i].get()) != &params[i]) 
                    return false;
        }
    }

    return true;
}



static bool reduce_control_flow(BB* i)
{
    bool changed = false;
    IRBuilder builder(i->get_parent()->get_context(), i);

    if (BrInst* br_inst = cast<BrInst>(&i->back()); br_inst->is_conditional())
    {
        if (is_redundant_cond_br(br_inst)) 
        {
            BrInst* jmp = builder.create_br(br_inst->get_successor(0));
            for (auto&& arg: br_inst->get_args(0)) {
                jmp->add_arg(0, arg);
            }

            br_inst->erase_from_parent();
            changed = true;
        }
    }

    if (BrInst* jmp = cast<BrInst>(&i->back()); jmp->is_unconditional())
    {
        BB* j = jmp->get_successor(0);

        if (i->size() == 1) 
        {
            if (just_forwarding(i)) {
                while (i->get_pred_num() > 0) {
                    BrInst& last = cast<BrInst>(i->pred_begin()->back());
                    if (last.is_conditional() && last.get_successor(1) == i)
                        last.set_successor(1, j);
                    else
                        last.set_successor(0, j);
                }

                for (int k = 0; k < i->param_size(); ++k)
                    i->get_params()[k].replace_all_uses_with(&j->get_params()[k]);

                i->erase_from_parent();
                changed = true;
                i = nullptr;
            }
            else if (i->param_size() == 0 && j->param_size() > 0 && i->get_pred_num() > 0) {
                while (i->get_pred_num() > 0) {
                    BrInst& last = cast<BrInst>(i->pred_begin()->back());
                    int idx = last.get_successor(0) == i ? 0 : 1;
                    last.set_successor(idx, j);

                    for (auto&& arg: jmp->get_args(0)) {
                        last.add_arg(idx, arg);
                    }
                }

                i->erase_from_parent();
                changed = true;
                i = nullptr;
            }
        }
        
        if (i && j->get_pred_num() == 1) 
        {
            auto args = jmp->get_args(0);
            for (int k = 0; k < j->param_size(); ++k)
                j->get_params()[k].replace_all_uses_with(args[k]);
            
            jmp->erase_from_parent();
           
            while (j->size() > 0) {
                Inst& inst = j->front();
                inst.move_before(i, i->end());
            }

            j->erase_from_parent();
            changed = true;
        }
        else if (i && j->size() == 1 && dyn_cast<BrInst>(&j->back()) && cast<BrInst>(j->back()).is_conditional())
        {
            BrInst* br = cast<BrInst>(&j->back());
            if (just_forwarding(j)) {
                Value* cond = br->get_condition();
                BB* target1 = br->get_successor(0);
                BB* target2 = br->get_successor(1);
                auto args = jmp->get_args(0);
                
                int idx = -1;
                if (BBParam* p = dyn_cast<BBParam>(cond); p && p->get_parent() == j) 
                    idx = p->get_index();
                
                BrInst* new_br = nullptr;
                if (idx == -1) 
                    new_br = builder.create_cond_br(cond, target1, target2);
                else 
                    new_br = builder.create_cond_br(args[idx], target1, target2);

                for (auto&& arg: args) {
                    new_br->add_arg(0, arg);
                    new_br->add_arg(1, arg);
                }

                jmp->erase_from_parent();
                changed = true;
            }
            else if (j->param_size() == 0 && (br->get_num_args(0) > 0 || br->get_num_args(1) > 0)) {
                BrInst* new_br = builder.create_cond_br(br->get_condition(), br->get_successor(0), br->get_successor(1));

                for (auto&& arg: br->get_args(0)) 
                    new_br->add_arg(0, arg);

                for (auto&& arg: br->get_args(1)) 
                    new_br->add_arg(1, arg);

                jmp->erase_from_parent();
                changed = true;
            }
        }
    }

    return changed;
}


static void reduce_control_flow(Function* fn)
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        POTraversal traversal(fn);
        for (auto bb = traversal.begin(); bb != traversal.end(); ++bb)
        {
            if (dyn_cast<BrInst>(&bb->back())) {
                changed = reduce_control_flow(to_address(bb)) || changed;
            }
        }
    }
}


void dead_code_elimination(Function* fn)
{
    PostDominatorTree tree(fn);
    auto [marked, useful_block] = mark(fn, tree);
    sweep(fn, marked, useful_block, tree);
    reduce_control_flow(fn);
}


void dead_code_elimination(Module* module)
{
    for (auto fn = module->begin(); fn != module->end(); ++fn)
        dead_code_elimination(to_address(fn));
}