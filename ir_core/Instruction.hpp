#ifndef PCC_IR_CORE_INSTRUCTION_H
#define PCC_IR_CORE_INSTRUCTION_H


#include "utils/ilist.hpp"
#include "User.hpp"


class BB;
class Function;


/**
 * @class Inst
 * @brief Represents an instruction in the intermediate representation.
 *
 * Inherits from the \c User class and the \c ilist_node class, which allows
 * instructions to be placed in a intrusive list.
 *
 * @note Only \c IRBuilder should create instances of this class.
 */
class Inst: public User, public ilist_node<Inst>
{
    friend class IRBuilder;
    friend class ilist<Inst>;

private:
    BB* parent;

protected:
    Inst() = delete;

    /**
     * @brief Constructor used to initialize an \c Inst object.
     * 
     * @param ty The type of the value this \c Inst represents.
     * @param kind The kind of value this \c Inst represents.
     * @param parent The basic block this instruction is a part of.
     * @param before The instruction before which this instruction should be inserted.
     */
    Inst(Type* ty, ValueKind kind, BB* parent, Inst* before);
    Inst(const Inst&) = delete;
    Inst& operator=(const Inst&) = delete;

    virtual ~Inst() = default;

public:
    /**
     * @brief Returns the parent basic block of this instruction.
     *
     * @return The parent basic block of this instruction.
     */
    BB* get_parent() noexcept {
        return parent;
    }

    const BB* get_parent() const noexcept {
        return parent;
    }

    /**
     * @brief Checks if this instruction is unary.
     *
     * @return True if this instruction is unary, false otherwise.
     */    
    bool is_unary() const noexcept {
        return get_kind() > ValueKind::INST_UNARY_BEGIN &&
               get_kind() < ValueKind::INST_UNARY_END;
    }

    /**
     * @brief Checks if this instruction is binary.
     *
     * @return True if this instruction is binary, false otherwise.
     */
    bool is_binary() const noexcept {
        return get_kind() > ValueKind::INST_BINARY_BEGIN &&
               get_kind() < ValueKind::INST_BINARY_END;
    }

    /**
     * @brief Checks if this instruction is binary.
     *
     * @return True if this instruction is binary, false otherwise.
     */
    bool is_terminator() const noexcept {
        return get_kind() > ValueKind::INST_TERM_BEGIN &&
               get_kind() < ValueKind::INST_TERM_END;
    }


    /// Create a copy of this instruction that is identical in all ways, 
    /// except the instruction has no parent.
    virtual Inst *clone() const;

    /// Insert an unlinked instruction into a basic block immediately before
    /// the specified instruction.
    void insert_before(Inst *pos);

    void insert_before(BB *bb, ilist<Inst>::iterator pos);

    /// Insert an unlinked instruction into a basic block immediately after the
    /// specified instruction.
    void insert_after(Inst *pos);

    /// @brief Removes this instruction from its parent basic block.
    /// @return An iterator pointing to the element after the erased one.
    ilist<Inst>::iterator erase_from_parent();

    /// This method unlinks 'this' from the containing basic block, but does not
    /// delete it.
    ilist<Inst>::iterator remove_from_parent();

    ilist<Inst>::iterator move_before(Inst *pos);

    ilist<Inst>::iterator move_before(BB *bb, ilist<Inst>::iterator pos);

    ilist<Inst>::iterator move_after(Inst *pos);

    /// @brief Checks if a \c Value object is an instance of the \c Inst class.
    /// @param v The \c Value object to check.
    /// @return True if the \c Value object is an instance of the \c Inst class, false otherwise.
    static bool classof(const Value* v) {
        return v->get_kind() > ValueKind::INST_BEGIN &&
               v->get_kind() < ValueKind::INST_END;
    }
};


/**
 * @class UnaryInst
 * @brief Represents a unary instruction in the intermediate representation (IR).
 * 
 * Unary instructions operate on a single operand.
 */
class UnaryInst: public Inst
{
    friend class IRBuilder;
protected:
    /**
     * @brief Constructor used to initialize an \c UnaryInst object.
     * 
     * @param kind The kind of value this \c UnaryInst represents.
     * @param src The source operand.
     * @param parent The basic block this instruction is a part of.
     * @param before The instruction before which this instruction should be inserted.
     */
    UnaryInst(ValueKind kind, Value* src, BB* parent, Inst* before);

public:
    static bool classof(const Value* v) {
        return v->get_kind() > ValueKind::INST_UNARY_BEGIN &&
               v->get_kind() < ValueKind::INST_UNARY_END;
    }
};


/**
 * @class LoadInst
 * @brief Represents a load instruction in the intermediate representation (IR).
 */
class LoadInst: public UnaryInst
{
    friend class IRBuilder;
private:
    LoadInst(Value* src, BB* parent, Inst* before): 
        UnaryInst(ValueKind::INST_LOAD, src, parent, before) 
    {
        set_type(src->get_type()->base);
    }

public:
    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::INST_LOAD;
    }
};


/**
 * @class CastInst
 * @brief Represents a cast instruction in the intermediate representation (IR).
 */
class CastInst: public UnaryInst
{
    friend class IRBuilder;
private:
    CastInst(Type* ty, Value* src, BB* parent, Inst* before):
        UnaryInst(ValueKind::INST_CAST, src, parent, before) 
    {
        set_type(ty);
    }

public:
    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::INST_CAST;
    }
};


/**
 * @class BinaryInst
 * @brief Represents a binary instruction in the intermediate representation (IR).
 */
class BinaryInst: public Inst
{
    friend class IRBuilder;
    
protected:
    BinaryInst(ValueKind kind, Value* lhs, Value* rhs, BB* parent, Inst* before);

public:
    static bool classof(const Value* v) {
        return v->get_kind() > ValueKind::INST_BINARY_BEGIN &&
               v->get_kind() < ValueKind::INST_BINARY_END;
    }
};


/**
 * @class CmpInst
 * @brief Represents a compare instruction in the intermediate representation (IR).
 */
class CmpInst: public BinaryInst
{
    friend class IRBuilder;
private:
    CmpInst(ValueKind kind, Value* lhs, Value* rhs, BB* parent, Inst* before): 
        BinaryInst(kind, lhs, rhs, parent, before) 
    {
        set_type(ty_bool);
    }
};


/**
 * @class RetInst
 * @brief Represents a return instruction in the intermediate representation (IR).
 */
class RetInst: public Inst
{
    friend class IRBuilder;
private:
    RetInst(Value* ret, BB* parent, Inst* before);

public:
    static bool classof(const Value *v) {
        return v->get_kind() == ValueKind::INST_RETURN;
    }
};


/**
 * @class AllocaInst
 * @brief Represents an allocation instruction in the intermediate representation (IR).
 */
class AllocaInst: public Inst
{
    friend class IRBuilder;
private:
    AllocaInst(Type* ty, BB* parent, Inst* before): 
        Inst(pointer_to(ty), ValueKind::INST_ALLOCA, parent, before) {}

public:
    static bool classof(const Value *v) {
        return v->get_kind() == ValueKind::INST_ALLOCA;
    }
};


/**
 * @class StoreInst
 * @brief Represents a store instruction in the intermediate representation (IR).
 */
class StoreInst: public Inst
{
    friend class IRBuilder;
private:
    StoreInst(Value* src, Value* dst, BB* parent, Inst* before);

public:
    static bool classof(const Value *v) {
        return v->get_kind() == ValueKind::INST_STORE;
    }
};


/**
 * @class BrInst
 * @brief Represents a branch instruction in the intermediate representation (IR).
 */
class BrInst: public Inst
{
    friend class IRBuilder;
private:
    int else_args_offset;

    /**
     * @brief Constructs a new unconditional \c BrInst object.
     * 
     * @param then_ Pointer to the \c BB (BasicBlock) that this branch instruction jumps to.
     * @param parent Pointer to the \c BB (BasicBlock) that contains this instruction.
     * @param before Insert this instruction before the instruction pointed to by \p before.
     * @param then_args A \c std::vector of \c Value pointers that represent the arguments for the next basic block.
     */
    BrInst(BB* then_, BB* parent, Inst* before, const std::vector<Value*>& then_args = {});

    /**
     * @brief Constructs a new conditional \c BrInst object.
     * 
     * @param cond The condition \c Value upon which the branch depends.
     * @param then_ Pointer to the \c BB (BasicBlock) that this branch instruction jumps to if the condition is true.
     * @param else_ Pointer to the \c BB (BasicBlock) that this branch instruction jumps to if the condition is false.
     * @param parent Pointer to the \c BB (BasicBlock) that contains this instruction.
     * @param before Insert this instruction before the \c Inst pointed to by \p before.
     * @param then_args A \c std::vector of \c Value pointers that represent the arguments for the then branch.
     * @param else_args A \c std::vector of \c Value pointers that represent the arguments for the else branch.
     */
    BrInst(Value* cond, BB* then_, BB* else_, BB* parent, Inst* before,
        const std::vector<Value*>& then_args = {}, 
        const std::vector<Value*>& else_args = {});


public:
    class succ_iterator: public iterator_adaptor<succ_iterator, op_iterator, BB>
    {
        friend class iterator_core_access;
        using super_t = iterator_adaptor<succ_iterator, op_iterator, BB>;

    public:
        using reference = super_t::reference;
        using iterator_category = typename super_t::iterator_category;

        succ_iterator() = default;
        explicit succ_iterator(op_iterator iter): super_t(iter) {}

    private:
        reference dereference() const {
            return *cast<BB>(base()->get());
        }
    };

    class const_succ_iterator: public iterator_adaptor<const_succ_iterator, const_op_iterator, const BB>
    {
        friend class iterator_core_access;
        using super_t = iterator_adaptor<const_succ_iterator, const_op_iterator, const BB>;

    public:
        using reference = super_t::reference;
        using iterator_category = typename super_t::iterator_category;

        const_succ_iterator() = default;
        explicit const_succ_iterator(const_op_iterator iter): super_t(iter) {}
        explicit const_succ_iterator(succ_iterator iter): super_t(iter.base()) {}

    private:
        reference dereference() const {
            return *cast<BB>(base()->get());
        }
    };

    /**
     * @brief Check if this branch is unconditional.
     * 
     * @return \c true if this branch is unconditional, \c false otherwise.
     */
    bool is_unconditional() const noexcept { return else_args_offset == -1; }
    
    /**
     * @brief Check if this branch is conditional.
     * 
     * @return \c true if this branch is conditional, \c false otherwise.
     */
    bool is_conditional() const noexcept { return else_args_offset != -1; }

    /**
     * @brief Get the condition of this branch.
     * 
     * @return The condition \c Value of this branch.
     */
    Value *get_condition() const {
        assert(is_conditional() && "Cannot get condition of an unconditional branch!");
        return get_operand(0);
    }

    /**
     * @brief Set the condition of this branch.
     * 
     * @param v The new condition \c Value of this branch.
     */
    void set_condition(Value *v) {
        assert(is_conditional() && "Cannot set condition of unconditional branch!");
        set_operand(0, v);
    }

    /**
     * @brief Get the successor of this branch
     * 
     * @param i The successor index
     * @return The then block if i == 0 or else block if i == 1
     */
    BB* get_successor(op_size_type i);
    const BB* get_successor(op_size_type i) const;

    void set_successor(op_size_type i, BB* bb);

    /**
     * @brief Get the range of successors for this branch instruction.
     * 
     * @return An \c iterator_range of successors.
     */
    iterator_range<succ_iterator> successors();
    iterator_range<const_succ_iterator> successors() const;

    virtual Inst *clone() const;

    /**
     * @brief Get the number of arguments for a successor basic block.
     * 
     * @param i The index of the successor. 0 for then block, 1 for else block.
     * @return The number of arguments for the successor.
     */
    op_size_type get_num_args(op_size_type i) const;

    /**
     * @brief Add an argument for a successor basic block.
     * 
     * @param i The index of the successor. 0 for then block, 1 for else block.
     * @param arg The argument to add.
     */    
    void add_arg(op_size_type i, Value* arg);

    void add_arg(op_size_type i, Value* arg, op_size_type loc);

    void remove_arg(op_size_type i, op_size_type idx);

    /**
     * @brief Get the range of arguments for a successor basic block.
     * 
     * @param i The index of the successor. 0 for then block, 1 for else block.
     * @return An \c iterator_range of arguments.
     */
    iterator_range<op_iterator> get_args(op_size_type i);
    iterator_range<const_op_iterator> get_args(op_size_type i) const;

    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::INST_BR;
    }
};


/**
 * @class CallInst
 * @brief Represents a function call instruction in the intermediate representation (IR).
 */
class CallInst: public Inst
{
    friend class IRBuilder;

private:
    /**
     * @brief Constructs a new \c CallInst object.
     * 
     * @param callee Pointer to the \c Function being called.
     * @param args A \c std::vector of \c Value pointers that represent the arguments to the function.
     * @param parent Pointer to the \c BB (BasicBlock) that contains this instruction.
     * @param before Insert this instruction before the instruction pointed to by \p before.
     */
    CallInst(Function* callee, const std::vector<Value*>& args, BB* parent, Inst* before);

public:
    /**
     * @brief Begin iterator to the function's arguments.
     * 
     * @return Iterator pointing to the start of the arguments.
     */
    op_iterator arg_begin() noexcept { return op_begin() + 1; }
    const_op_iterator arg_begin() const noexcept { return op_begin() + 1; }

    /**
     * @brief End iterator to the function's arguments.
     * 
     * @return Iterator pointing to the end of the arguments.
     */
    op_iterator arg_end() noexcept { return op_end(); }
    const_op_iterator arg_end() const noexcept { return op_end(); }

    iterator_range<op_iterator> args() {
        return make_range(arg_begin(), arg_end());
    }
    iterator_range<const_op_iterator> args() const {
        return make_range(arg_begin(), arg_end());
    }

    /// Checks if the function has any arguments.
    bool arg_empty() const noexcept { return arg_end() == arg_begin(); }

    /// Gets the number of arguments to the function.
    op_size_type arg_size() const noexcept { return arg_end() - arg_begin(); }

    /**
     * @brief Retrieves the argument at the specified index.
     * 
     * @param i Index of the argument to retrieve.
     * @return Pointer to the argument value.
     */
    Value *get_arg(op_size_type i) const {
        assert(i < arg_size() && "Out of bounds!");
        return get_operand(i + 1);
    }

    /**
     * @brief Sets the argument at the specified index.
     * 
     * @param i Index of the argument to set.
     * @param v Pointer to the new argument value.
     */
    void set_arg(op_size_type i, Value *v) {
        assert(i < arg_size() && "Out of bounds!");
        set_operand(i + 1, v);
    }

    /// @brief Retrieves the function being called.
    /// @return Pointer to the called function.
    Function* get_called_function() const noexcept { 
        return cast<Function>(get_operand(0).get()); 
    }

    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::INST_CALL;
    }
};




#endif /* PCC_IR_CORE_INSTRUCTION_H */
