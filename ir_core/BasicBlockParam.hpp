#ifndef PCC_IR_CORE_BASICBLOCKPARAM_H
#define PCC_IR_CORE_BASICBLOCKPARAM_H

#include "Value.hpp"


/**
 * @class BBParam
 * @brief Represents a parameter of a basic block in the control flow graph (CFG).
 * 
 * The \c BBParam class is a specialized kind of value that is used to represent 
 * parameters of basic blocks. It keeps a reference to its parent basic block 
 * and maintains its position within the parameter list of the basic block.
 */
class BBParam: public Value
{
    friend class BB;

private:
    BB* parent;
    std::size_t index;

    /**
     * @brief Constructs a BBParam with the given type, parent block, and index.
     * 
     * @param ty The type of the parameter.
     * @param parent The parent basic block to which this parameter belongs.
     * @param index The position of this parameter within the parent block's parameter list.
     */
    BBParam(Type* ty, BB* parent, std::size_t index): 
        Value(ty, ValueKind::BB_PARAM), parent(parent), index(index) {}
    
    /**
     * @brief Updates the index of this parameter.
     */
    void set_index(std::size_t new_index) noexcept { index = new_index; }

public:
    /**
     * @brief Gets the parent basic block to which this parameter belongs.
     * 
     * @return A pointer to the parent basic block.
     */
    BB* get_parent() noexcept { return parent; }
    BB* get_parent() const noexcept { return parent; }

    /**
     * @brief Retrieves the index of this parameter within the parent block's parameter list.
     * 
     * @return The index of the parameter.
     */
    std::size_t get_index() const noexcept { return index; }

    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::BB_PARAM;
    }
};



#endif /* PCC_IR_CORE_BASICBLOCKPARAM_H */
