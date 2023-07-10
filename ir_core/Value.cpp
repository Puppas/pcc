#include "Value.hpp"
#include "User.hpp"


void Value::replace_all_uses_with(Value* val)
{
    for (auto&& user = user_begin(); user != user_end(); ) {
        auto old_user = user++;
        for (auto&& op: old_user->get_operands()) {
            if (op == this) {
                op = val;
            }
        }
    }
}