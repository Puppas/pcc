#include "Constant.hpp"
#include "IRContext.hpp"


ConstantInt* ConstantInt::get(IRContext& context, int64_t val)
{
    return context.get_constant(val);
}