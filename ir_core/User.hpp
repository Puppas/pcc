#ifndef PCC_IR_CORE_USER_H
#define PCC_IR_CORE_USER_H


#include <vector>
#include <memory>
#include "Use.hpp"


/**
 * @class User
 * @brief Represents a \c Value that also uses other Values.
 * 
 * @details This class is a sub-class of \c Value and is used to represent Values that are
 *          also using other Values. It maintains a list of \c Use objects representing the
 *          operand Values it is using.
 */
class User: public Value
{
public:
    using op_list = std::vector<Use*>;
    using op_iterator = indirect_iterator<op_list::iterator, Use>;
    using const_op_iterator = indirect_iterator<op_list::const_iterator, const Use>;
    using op_size_type = typename op_list::size_type;

private:
    op_list ops;

protected:
    /**
     * @brief Construct a new \c User object.
     *
     * @param ty Type of the \c Value this \c User represents.
     * @param kind The kind of \c Value this \c User represents.
     */
    User(Type* ty, ValueKind kind): Value(ty, kind) {}

    /**
     * @brief Destroy the \c User object.
     *
     * @details Destructor ensures all associated \c Use objects are also deleted.
     */
    ~User() {
        for (auto iter = ops.begin(); iter != ops.end(); ++iter) {
            delete *iter;
        }
    }

    void add_operand(Value* op) {
        ops.push_back(new Use(this, op));
    }

    void add_operand(op_iterator before, Value* op) {
        ops.insert(before.base(), new Use(this, op));
    }

    void remove_operand(op_size_type i) {
        set_operand(i, nullptr);
        ops.erase(ops.begin() + i);
    }

public:
    /**
     * @brief Returns an iterator pointing to the beginning of the operand list.
     *
     * @return An iterator pointing to the beginning of the operand list.
     */
    op_iterator op_begin() noexcept { 
        return make_indirect_iterator(ops.begin()); 
    }

    /**
     * @brief Returns an iterator pointing to the end of the operand list.
     *
     * @return An iterator pointing to the end of the operand list.
     */
    op_iterator op_end() noexcept { 
        return make_indirect_iterator(ops.end()); 
    }

    /**
     * @brief Returns an iterator pointing to the end of the operand list.
     *
     * @return An iterator pointing to the end of the operand list.
     */
    const_op_iterator op_begin() const noexcept { 
        return make_indirect_iterator(ops.begin()); 
    }

     /**
     * @brief Returns a const iterator pointing to the end of the operand list.
     *
     * @return A const iterator pointing to the end of the operand list.
     */   
    const_op_iterator op_end() const noexcept { 
        return make_indirect_iterator(ops.end()); 
    }

    /**
     * @brief Returns the number of operands in the operand list.
     *
     * @return The number of operands in the operand list.
     */
    op_size_type get_num_operands() const noexcept {
        return ops.size();
    }

    /**
     * @brief Accesses an operand at a given position.
     *
     * @param i The position of the operand to access.
     * @return The operand at the given position.
     */
    Use& get_operand(op_size_type i) { 
        assert(i < ops.size());
        return *ops[i]; 
    }

    /**
     * @brief Returns a const reference to the operand at a given position.
     *
     * @param i The position of the operand to access.
     * @return A const reference to the operand at the given position.
     */
    const Use& get_operand(op_size_type i) const { 
        assert(i < ops.size());
        return *ops[i]; 
    }

    /**
     * @brief Replaces the operand at a given position with a new operand.
     *
     * @param i The position of the operand to replace.
     * @param v The new operand to set.
     */
    void set_operand(op_size_type i, Value* v) {
        *ops[i] = v;
    }

    /**
     * @brief Returns an iterator range over the operands.
     *
     * @return An iterator range over the operands.
     */
    iterator_range<op_iterator> get_operands() {
        return make_range(op_begin(), op_end());
    }

    /**
     * @brief Returns a const iterator range over the operands.
     *
     * @return A const iterator range over the operands.
     */
    iterator_range<const_op_iterator> get_operands() const {
        return make_range(op_begin(), op_end());
    }

    /**
     * @brief Removes all operands from the \c User object.
     * 
     * This method is typically used before removing the \c User.
     */
    void drop_all_references() {
        for (auto &&op : get_operands())
            op.set(nullptr);
    }
};




#endif /* PCC_IR_CORE_USER_H */
