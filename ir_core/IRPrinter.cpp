#include "IRPrinter.hpp"
#include "Module.hpp"
#include "utils/util.hpp"
#include "iterator/to_address.hpp"


std::string IRPrinter::op_to_str(ValueKind kind)
{
    switch (kind)
    {
    case ValueKind::INST_ADD:
        return "add";
    case ValueKind::INST_SUB:
        return "sub";
    case ValueKind::INST_NEG:
        return "neg";
    case ValueKind::INST_MUL:
        return "mul";
    case ValueKind::INST_DIV:
        return "div";
    case ValueKind::INST_MOD:
        return "mod";
    case ValueKind::INST_EQ:
        return "eq";
    case ValueKind::INST_NE:
        return "ne";
    case ValueKind::INST_LT:
        return "lt";
    case ValueKind::INST_LE:
        return "le";
    case ValueKind::INST_BITAND:
        return "bitand";
    case ValueKind::INST_BITOR:
        return "bitor";
    case ValueKind::INST_BITXOR:
        return "bitxor";
    case ValueKind::INST_BITNOT:
        return "bitnot";
    case ValueKind::INST_RETURN:
        return "ret";
    case ValueKind::INST_CALL:
        return "call";
    case ValueKind::INST_BR:
        return "br";
    case ValueKind::INST_LOAD:
        return "load";
    case ValueKind::INST_STORE:
        return "store";
    case ValueKind::INST_ALLOCA:
        return "alloca";
    case ValueKind::INST_CAST:
        return "cast";
    default:
        break;
    }

    unreachable();
}

std::string IRPrinter::ty_to_str(Type *ty)
{
    switch (ty->kind)
    {
    case TY_BOOL:
        return "bool";
    case TY_CHAR:
        return "char";
    case TY_SHORT:
        return "short";
    case TY_INT:
        return "int";
    case TY_LONG:
        return "long";
    case TY_PTR:
        return "ptr";
    default:
        break;
    }

    unreachable();
}

std::string IRPrinter::val_to_str(const Value *v)
{
    if (const ConstantInt* const_int = dyn_cast<const ConstantInt>(v)) {
        return ty_to_str(const_int->get_type()) + " " + std::to_string(const_int->get_value());
    }

    if (const GlobalObject * g = dyn_cast<const GlobalObject>(v)) {
        return ty_to_str(g->get_type()) + " @" + g->get_name();
    }

    if (val_to_num.find(v) == val_to_num.end()) {
        val_to_num[v] = next_num;
        ++next_num;
    }

    if (isa<BB>(v)) {
        return "%" + std::to_string(val_to_num[v]);
    }

    return ty_to_str(v->get_type()) + " %" + std::to_string(val_to_num[v]);
}

std::string IRPrinter::inst_to_str(const Inst *inst)
{
    std::string res;

    if (dyn_cast<const AllocaInst>(inst)) {
        std::string lhs = val_to_str(inst);
        std::string rhs = op_to_str(inst->get_kind()) + " " + ty_to_str(inst->get_type()->base);
        res = lhs + " = " + rhs;
    }
    else if (const BrInst *br_inst = dyn_cast<const BrInst>(inst)) {
        if (br_inst->is_conditional()) {
            res = op_to_str(br_inst->get_kind()) + " " + val_to_str(br_inst->get_condition()) + 
                  ", label: " + val_to_str(br_inst->get_operand(1)) + " ";
        }
        else {
            res = op_to_str(br_inst->get_kind()) + " label: " + val_to_str(br_inst->get_operand(0)) + " ";
        }

        if (br_inst->get_num_args(0) > 0) {
            res += "(";
            for (auto &&arg : br_inst->get_args(0)) {
                res += val_to_str(arg) + ", ";
            }
            res.erase(res.end() - 2, res.end());
            res += ")";
        }

        if (br_inst->is_conditional()) {
            res += ", label: " + val_to_str(br_inst->get_operand(2)) + " ";
            if (br_inst->get_num_args(1) > 0) {
                res += "(";
                for (auto &&arg : br_inst->get_args(1)) {
                    res += val_to_str(arg) + ", ";
                }
                res.erase(res.end() - 2, res.end());
                res += ")";
            }
        }
    }
    else {
        std::string rhs = op_to_str(inst->get_kind()) + " ";

        for (int i = 0; i < inst->get_num_operands(); ++i) {
            rhs += val_to_str(inst->get_operand(i));
            if (i + 1 < inst->get_num_operands()) {
                rhs += ", ";
            }
        }

        if (inst->get_type() == ty_void) {
            res = rhs;
        }
        else {
            std::string lhs = val_to_str(inst);
            res = lhs + " = " + rhs;
        }
    }

    return res;
}


void IRPrinter::print(const BB *bb, std::ostream &os)
{
    std::string header = val_to_str(bb);

    if (bb->param_size() > 0) {
        header.push_back('(');
        for (auto iter = bb->param_begin(); iter != bb->param_end(); ++iter)
            header += val_to_str(to_address(iter)) + ", ";
        
        header.erase(header.size() - 2);
        header.push_back(')');
    }

    header += ":";

    if (!bb->predecessors().empty()) {
        header += "\tpreds = ";
        for (auto&& pred: bb->predecessors()) {
          header += val_to_str(&pred) + ", ";
        }
        header.erase(header.size() - 2);          
    }

    os << header << "\n";
    for (auto iter = bb->begin(); iter != bb->end(); ++iter) {
        os << "  " << inst_to_str(to_address(iter)) << '\n';
    }
    os << '\n';
}



void IRPrinter::print(const Function *func, std::ostream &os)
{
    val_to_num.clear();
    next_num = 0;

    std::string decl = "define " + ty_to_str(func->get_return_type()) + " @" + func->get_name() + "(";

    for (auto iter = func->param_begin(); iter != func->param_end(); ++iter) {
        decl += val_to_str(to_address(iter));
        if (iter + 1 < func->param_end()) {
            decl += ", ";
        }
    }

    decl += ")";
    os << decl << " {\n";

    for (auto iter = func->begin(); iter != func->end(); ++iter) {
        print(to_address(iter), os);
    }

    os << "}\n\n";
    return;
}

void IRPrinter::print(const Module *m, std::ostream &os)
{
    for (auto gvar = m->global_begin(); gvar != m->global_end(); ++gvar)
    {
        os << "@" << gvar->get_name() << " =  global " << gvar->get_type()->base << "\n";
    }

    for (auto func = m->begin(); func != m->end(); ++func)
    {
        print(to_address(func), os);
    }
}
