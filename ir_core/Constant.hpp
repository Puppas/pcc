#ifndef PCC_IR_CORE_CONSTANT_H
#define PCC_IR_CORE_CONSTANT_H


#include "Value.hpp"

class IRContext;



/**
 * @class Constant
 * @brief This class represents a constant value in the intermediate representation.
 * 
 * Constants are immutable during the execution of a program and may be used as the operands 
 * for instructions. This class is designed as a base class for different types of constants,
 * such as integer constants, floating-point constants, etc.
 */
class Constant: public Value
{
protected:
    Constant() = delete;
    Constant(Type* ty, ValueKind kind): Value(ty, kind) {}
    Constant(const Constant&) = delete;
    Constant& operator=(const Constant&) = delete;
    ~Constant() = default;

public:
    static bool classof(const Value* v) {
        return v->get_kind() > ValueKind::CONSTANT_BEGIN &&
               v->get_kind() < ValueKind::CONSTANT_END;
    }

};


/**
 * @class ConstantInt
 * @brief This class represents a constant integer value in the intermediate representation.
 * 
 * It inherits from the \c Constant base class and adds an integer value that this constant represents.
 */
class ConstantInt: public Constant
{
    friend class IRContext;
private:
    std::int64_t val;

    ConstantInt() = delete;
    ConstantInt(Type* ty, std::int64_t val): 
        Constant(ty, ValueKind::CONSTANT_INT), val(val) {}

    ConstantInt(const ConstantInt&) = delete;
    ConstantInt& operator=(const ConstantInt&) = delete;
    ~ConstantInt() = default;

public:
    /**
     * @brief Creates a new instance of \c ConstantInt.
     * 
     * @param context The context in which to create the \c ConstantInt.
     * @param val The integer value that the \c ConstantInt should represent.
     * @return A pointer to the newly created \c ConstantInt.
     */
    static ConstantInt* get(IRContext& context, std::int64_t val);

    /**
     * @brief Gets the integer value that this constant represents.
     * 
     * @return The integer value.
     */
    auto get_value() const noexcept {
        return val;
    }

    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::CONSTANT_INT;
    }
};




#endif /* PCC_IR_CORE_CONSTANT_H */
