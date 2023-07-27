#ifndef PCC_IR_CORE_FUNCTIONPARAM_H
#define PCC_IR_CORE_FUNCTIONPARAM_H


#include "Value.hpp"


class FunctionParam: public Value
{
    friend class Function;

private:
    Function* parent;

    FunctionParam(Type* ty, Function* parent):
        Value(ty, ValueKind::FUNCTION_PARAM), parent(parent) {}

public:
    Function* get_parent() noexcept { return parent; }
    Function* get_parent() const noexcept { return parent; }

    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::FUNCTION_PARAM;
    }
};

#endif /* PCC_IR_CORE_FUNCTIONPARAM_H */
