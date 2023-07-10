#include "IRContext.hpp"


IRContext::~IRContext()
{
    while (!modules.empty())
        delete *modules.begin();
    
    while (!int_constants.empty()) {
        delete int_constants.begin()->second;
        int_constants.erase(int_constants.begin());
    }
}

ConstantInt* IRContext::get_constant(std::int64_t val) {
    auto iter = int_constants.find(val);
    if (iter != int_constants.end()) {
        return iter->second;
    }

    Type* ty = val == (std::int32_t)val ? ty_int : ty_long;
    int_constants[val] = new ConstantInt(ty, val);
    return int_constants[val];
}

