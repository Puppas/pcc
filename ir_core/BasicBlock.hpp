#ifndef PCC_IR_CORE_BASICBLOCK_H
#define PCC_IR_CORE_BASICBLOCK_H


#include <vector>
#include "utils/ilist.hpp"
#include "Instruction.hpp"
#include "BasicBlockParam.hpp"


class Function;


/**
 * @class BB
 * @brief This class represents a basic block in a control flow graph (CFG) of a function.
 * 
 * This class also extends from \c Value and \c ilist_node, allowing it to be used as an operand for 
 * instructions and to be part of an intrusive linked list respectively. It contains an 
 * instruction list and a parameter list. The instruction list contains the set of 
 * instructions that belong to this basic block. The parameter list contains the set of 
 * parameters passed to this basic block.
 * 
 */
class BB: public Value, public ilist_node<BB>
{
    friend class Inst;
    friend class ilist<BB>;

public:
    using inst_list = ilist<Inst>;
    using param_list = std::vector<BBParam*>;
    using iterator = typename inst_list::iterator;
    using const_iterator = typename inst_list::const_iterator;
    using reverse_iterator = typename inst_list::reverse_iterator;
    using const_reverse_iterator = typename inst_list::const_reverse_iterator;
    using size_type = typename inst_list::size_type;
    using param_iterator = indirect_iterator<param_list::iterator, BBParam>;
    using const_param_iterator = indirect_iterator<param_list::const_iterator, const BBParam>;
    using succ_iterator = BrInst::succ_iterator;
    using const_succ_iterator = BrInst::const_succ_iterator;

    class pred_iterator: public iterator_adaptor<pred_iterator, user_iterator, BB>
    {
        friend class iterator_core_access;
        using super_t = iterator_adaptor<pred_iterator, user_iterator, BB>;

    public:
        using reference = typename super_t::reference;
        using iterator_category = typename super_t::iterator_category;

    public:
        pred_iterator() = default;
        explicit pred_iterator(user_iterator iter): super_t(iter) {}

    private:
        reference dereference() const {
            return *(cast<BrInst>(*base()).get_parent());
        }
    };

    class const_pred_iterator: public iterator_adaptor<const_pred_iterator, const_user_iterator, const BB>
    {
        friend class iterator_core_access;
        using super_t = iterator_adaptor<const_pred_iterator, const_user_iterator, const BB>;

    public:
        using reference = super_t::reference;
        using iterator_category = typename super_t::iterator_category;

        const_pred_iterator() = default;
        explicit const_pred_iterator(const_user_iterator iter): super_t(iter) {}
        explicit const_pred_iterator(pred_iterator iter): super_t(iter.base()) {}

    private:
        reference dereference() const {
            return *(cast<const BrInst>(*base()).get_parent());
        }
    };

private:
    inst_list insts;
    param_list params;
    Function* parent;

private:
    BB() = delete;
    BB(Function *parent, BB* before);
    BB(const BB&) = delete;
    BB& operator=(const BB&) = delete;
    ~BB();

    inst_list& get_inst_list() { return insts; }
    const inst_list& get_inst_list() const { return insts; }

public:
    /**
     * @brief Creates a new basic block.
     * 
     * @param parent The function that this basic block will belong to.
     * @param before The basic block to insert the newly created basic block before. If null, 
     *               the basic block is added at the end.
     * @return A pointer to the newly created basic block.
     */
    static BB* create(Function* parent, BB* before = nullptr) {
        return new BB(parent, before);
    }

    /**
     * @brief Gets the parent function of this basic block.
     * 
     * @return A pointer to the parent function.
     */
    Function* get_parent() noexcept { return parent; }
    Function* get_parent() const noexcept { return parent; }

    // Iterator functions for the list of instructions in this basic block.
    iterator begin() noexcept { return insts.begin(); }
    iterator end() noexcept { return insts.end(); }

    const_iterator begin() const noexcept { return insts.begin(); }
    const_iterator end() const noexcept { return insts.end(); }

    reverse_iterator rbegin() noexcept { return insts.rbegin(); }
    reverse_iterator rend() noexcept { return insts.rend(); }

    const_reverse_iterator rbegin() const noexcept { return insts.rbegin(); }
    const_reverse_iterator rend() const noexcept { return insts.rend(); }
    
    /**
     * @brief Gets the number of parameters for this basic block.
     * 
     * @return The number of parameters.
     */
    param_list::size_type param_size() const noexcept { return params.size(); }

    param_iterator param_begin() noexcept { return make_indirect_iterator(params.begin()); }
    param_iterator param_end() noexcept { return make_indirect_iterator(params.end()); }
    const_param_iterator param_begin() const noexcept { return make_indirect_iterator(params.begin()); }
    const_param_iterator param_end() const noexcept { return make_indirect_iterator(params.end()); }

    iterator_range<param_iterator> get_params() {
        return make_range(param_begin(), param_end());
    }

    iterator_range<const_param_iterator> get_params() const {
        return make_range(param_begin(), param_end());
    }


    /**
     * @brief Inserts a new parameter of the given type into the parameter list of this basic block.
     * 
     * @param ty The type of the new parameter.
     * @return A pointer to the newly inserted parameter.
     */
    BBParam* insert_param(Type* ty);

    /**
     * @brief Erases the given paramter in the parameter list
     * 
     * @param idx The index of paramter to be erased 
     * @return An iterator pointing to the parameter following the erased one
     */
    param_iterator erase_param(param_list::size_type idx);

    Inst& front() { return insts.front(); }
    const Inst& front() const { return insts.front(); }

    Inst& back() { return insts.back(); }
    const Inst& back() const { return insts.back(); }

    /**
     * @brief Gets the number of instructions in this basic block.
     * 
     * @return The number of instructions.
     */
    size_type size() const noexcept { return insts.size(); }

    pred_iterator pred_begin() noexcept {
        return pred_iterator(user_begin());
    }

    const_pred_iterator pred_begin() const noexcept {
        return const_pred_iterator(user_begin());
    }

    pred_iterator pred_end() noexcept {
        return pred_iterator(user_end());
    }

    const_pred_iterator pred_end() const noexcept {
        return const_pred_iterator(user_end());
    }

    /// @brief Gets the number of predecessors
    auto get_pred_num() const noexcept { return users.size(); }

    /**
     * @brief Returns an \c iterator_range over the list of predecessor basic blocks of this basic block.
     * 
     * @return An \c iterator_range of predecessor basic blocks.
     */
    iterator_range<pred_iterator> predecessors() {
        return make_range(pred_begin(), pred_end());
    }

    iterator_range<const_pred_iterator> predecessors() const {
        return make_range(pred_begin(), pred_end());
    }

    succ_iterator succ_begin() { return successors().begin(); }
    const_succ_iterator succ_begin() const { return successors().begin(); }

    succ_iterator succ_end() { return successors().end(); }
    const_succ_iterator succ_end() const { return successors().end(); }

    /**
     * @brief Returns an \c iterator_range over the list of successor basic blocks of this basic block.
     * 
     * @return An \c iterator_range of successor basic blocks.
     */
    iterator_range<succ_iterator> successors();
    
    iterator_range<const_succ_iterator> successors() const;

    /// @brief Removes all the references to this basic block.
    void drop_all_references();

    /**
     * @brief Inserts this basic block before the specified basic block.
     * 
     * @param pos The reference basic block before which this block will be inserted.
     */
    void insert_before(BB *pos);

    /**
     * @brief Inserts this basic block after the specified basic block.
     * 
     * @param pos The reference basic block after which this block will be inserted.
     */
    void insert_after(BB *pos);

    /// @brief Removes this basic block from its parent function.
    /// @return An iterator pointing to the element after the erased one.
    ilist<BB>::iterator erase_from_parent();

    /// This method unlinks 'this' from the parent function, but does not
    /// delete it.
    ilist<BB>::iterator remove_from_parent();

    /**
     * @brief Moves this basic block before the specified basic block.
     * 
     * @param pos The reference basic block before which this block will be moved.
     * @return An iterator pointing to the position after the erased one.
     */
    ilist<BB>::iterator move_before(BB *pos);

    /**
     * @brief Moves this basic block after the specified basic block.
     * 
     * @param pos The reference basic block after which this block will be moved.
     * @return An iterator pointing to the position after the erased one.
     */
    ilist<BB>::iterator move_after(BB *pos);

    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::BB;
    }

    /**
     * @brief Prints the representation of the basic block to the given output stream.
     * 
     * @param os The output stream to print to.
     * @param debug A flag indicating whether to print debug information.
     */
    void print(std::ostream& os, bool debug = false) const;

    /**
     * @brief Overloaded operator for printing the basic block to an output stream.
     * 
     * @param os The output stream.
     * @param bb The basic block to be printed.
     * 
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const BB& bb);
};




#endif /* PCC_IR_CORE_BASICBLOCK_H */
