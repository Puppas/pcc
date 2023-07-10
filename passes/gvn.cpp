#include "ir_core/Module.hpp"
#include "utils/util.hpp"
#include "ir_core/Dominators.hpp"
#include "gvn.hpp"


static bool is_arithmetic(Inst* inst)
{
    return inst->get_kind() == ValueKind::INST_NEG || 
            inst->get_kind() == ValueKind::INST_BITNOT ||
           (inst->get_kind() > ValueKind::INST_BINARY_BEGIN && 
            inst->get_kind() < ValueKind::INST_BINARY_END);
}


static bool is_const_expr(Inst* inst)
{
    if (!is_arithmetic(inst))
        return false;

    if (UnaryInst* ui = dyn_cast<UnaryInst>(inst)) {
        return isa<Constant>(ui->get_operand(0).get());
    }

    BinaryInst* bi = dyn_cast<BinaryInst>(inst);
    return isa<Constant>(bi->get_operand(0).get()) && isa<Constant>(bi->get_operand(1).get());
}


static ConstantInt* get_const_val(Inst* inst, IRContext& context)
{
    std::int64_t val = 0;
    if (inst->is_unary())
    {
        val = cast<ConstantInt>(inst->get_operand(0).get())->get_value();
        switch (inst->get_kind()) {
        case ValueKind::INST_NEG:
            val = -val;
            break;
        case ValueKind::INST_BITNOT:
            val = ~val;       
            break;         
        default:
            unreachable();
        }
    }
    else 
    {
        std::int64_t lhs = cast<ConstantInt>(inst->get_operand(0).get())->get_value();
        std::int64_t rhs = cast<ConstantInt>(inst->get_operand(1).get())->get_value();

        switch (inst->get_kind()) {
        case ValueKind::INST_ADD:
            val = lhs + rhs;
            break;
        case ValueKind::INST_SUB:
            val = lhs - rhs;
            break;
        case ValueKind::INST_MUL:
            val = lhs * rhs;
            break;
        case ValueKind::INST_DIV:
            val = lhs / rhs;
            break;
        case ValueKind::INST_EQ:
            val = lhs == rhs;
            break;
        case ValueKind::INST_NE:
            val = lhs != rhs;
            break;
        case ValueKind::INST_LE:
            val = lhs <= rhs;
            break;
        case ValueKind::INST_LT:
            val = lhs < rhs;
            break;
        case ValueKind::INST_BITAND:
            val = lhs & rhs;
            break;
        case ValueKind::INST_BITOR:
            val = lhs | rhs;
            break;
        case ValueKind::INST_BITXOR:
            val = lhs ^ rhs;
            break;
        case ValueKind::INST_MOD:
            val = lhs % rhs;
            break;
        default:
            unreachable();
        }
    }

    return ConstantInt::get(context, val);
}

struct expr_record
{
    ValueKind kind;
    Value* lhs;
    Value* rhs;

    expr_record(Inst* inst): kind(inst->get_kind()), 
        lhs(inst->get_operand(0)),
        rhs(inst->is_binary() ? inst->get_operand(1).get() : nullptr) {}

    bool operator==(const expr_record& other) const {
        return kind == other.kind && lhs == other.lhs && rhs == other.rhs;
    }
};


struct expr_record_hash 
{
    std::size_t operator() (const expr_record& record) const
    {
        std::size_t h1 = std::hash<int>{}((int)(record.kind)); 
        std::size_t h2 = std::hash<Value*>{}(record.lhs); 
        std::size_t h3 = std::hash<Value*>{}(record.rhs); 
        return record.rhs ? h1 ^ (h2 << 1) ^ (h3 << 2) : h1 ^ (h2 << 1); 
    }
};


static void global_value_numbering(DomTreeNode* node, IRContext& context, 
    std::unordered_map<expr_record, Value*, expr_record_hash> expr_to_value)
{
    // For each instruction in the basic block
    for (auto inst = node->get_block()->begin(); inst != node->get_block()->end(); )
    {
        // Check if the instruction is a constant expression
        if (is_const_expr(to_address(inst))) {
            // Compute the constant value
            ConstantInt* const_val = get_const_val(to_address(inst), context);
            inst->replace_all_uses_with(const_val);
            inst = inst->erase_from_parent();
        }
        else if (is_arithmetic(to_address(inst))) {
            // Construct the key for the instruction
            expr_record key(to_address(inst));

            // If the key already exists in the map, replace the instruction with the existing value
            if (expr_to_value.find(key) != expr_to_value.end()) {
                inst->replace_all_uses_with(expr_to_value[key]);
                inst = inst->erase_from_parent();
            }
            else {
                // Otherwise, assign the new value number to the instruction
                expr_to_value[key] = to_address(inst);
                ++inst;
            }
        }
        else {
            ++inst;
        }
    }

    for (auto&& child: node->get_children()) {
        global_value_numbering(&child, context, expr_to_value);
    }
}


void global_value_numbering(Function* fn)
{
    // Create a map to store the value numbers
    std::unordered_map<expr_record, Value*, expr_record_hash> expr_to_value;
    IRContext& context = fn->get_context();
    DominatorTree tree(fn);
    global_value_numbering(tree.get_root(), context, expr_to_value);
}


void global_value_numbering(Module* module)
{
    for (auto fn = module->begin(); fn != module->end(); ++fn)
        global_value_numbering(to_address(fn));
}