#ifndef PCC_IR_CORE_FUNCTION_H
#define PCC_IR_CORE_FUNCTION_H


#include "GlobalObject.hpp"
#include "BasicBlock.hpp"
#include "symbol_table_list.hpp"


/**
 * @class Function
 * @brief This class represents a function in the intermediate representation.
 * 
 * Function in intermediate representation contains a list of Basic Blocks (BBs)
 * which represents the control flow graph of the function.
 */
class Function: public GlobalObject, public ilist_node<Function>
{
    friend class BB;
    friend class ilist<Function>;

public:
    using bb_list = ilist<BB>; ///< Type definition for list of basic blocks.
    using param_list = std::vector<Value*>; ///< Type definition for list of function parameters.
    using iterator = bb_list::iterator; ///< Iterator for basic block list.
    using const_iterator = bb_list::const_iterator; ///< Const iterator for basic block list.
    using param_iterator = indirect_iterator<param_list::iterator, Value>; ///< Iterator for function parameters list.
    using const_param_iterator = indirect_iterator<param_list::const_iterator, const Value>; ///< Const iterator for function parameters list.
    using size_type = bb_list::size_type; ///< Size type for basic block list.

private:
    bb_list bbs; ///< List of basic blocks within the function.
    param_list params; ///< List of function parameters.

private:
    /**
     * @brief Constructs a new \c Function.
     * 
     * @param ty The type of the function.
     * @param name The name of the function.
     * @param parent The module in which to create the function.
     */
    Function(Type* ty, const std::string& name, Module* parent);
    Function(const Function&) = delete;
    Function& operator=(const Function&) = delete;
    ~Function();

    bb_list& get_bb_list() { return bbs; }
    const bb_list& get_bb_list() const { return bbs; }

    void build_params();

public:
    /**
     * @brief Creates a new \c Function.
     * 
     * @param ty The type of the function.
     * @param name The name of the function.
     * @param parent The module in which to create the function.
     * 
     * @return Pointer to the newly created function.
     */
    static Function *create(Type *ty, const std::string& name, Module *parent) {
        return new Function(ty, name, parent);
    }

    /**
     * @brief Returns an iterator pointing to the first basic block in the function.
     * 
     * @return Iterator pointing to the first basic block.
     */
    iterator begin() noexcept { return bbs.begin(); }
    
    /**
     * @brief Returns a const iterator pointing to the first basic block in the function.
     * 
     * @return Const iterator pointing to the first basic block.
     */
    const_iterator begin() const noexcept { return bbs.begin(); }

    /**
     * @brief Returns an iterator pointing to the end of the basic blocks list in the function.
     * 
     * @return Iterator pointing to the end of the basic blocks list.
     */
    iterator end() noexcept { return bbs.end(); }

    /**
     * @brief Returns a const iterator pointing to the end of the basic blocks list in the function.
     * 
     * @return Const iterator pointing to the end of the basic blocks list.
     */
    const_iterator end() const noexcept { return bbs.end(); }

    /**
     * @brief Returns the number of basic blocks in the function.
     * 
     * @return The number of basic blocks.
     */
    size_type size() const noexcept { return bbs.size(); }

    /**
     * @brief Checks if the function has no basic blocks.
     * 
     * @return true if the function has no basic blocks, false otherwise.
     */
    bool empty() const noexcept { return bbs.empty(); }

    /**
     * @brief Returns a reference to the first basic block in the function.
     * 
     * @return Reference to the first basic block.
     */
    BB& front() { return bbs.front(); }

    /**
     * @brief Returns a constant reference to the first basic block in the function.
     * 
     * @return Constant reference to the first basic block.
     */
    const BB& front() const { return bbs.front(); }

    /**
     * @brief Returns a reference to the last basic block in the function.
     * 
     * @return Reference to the last basic block.
     */
    BB& back() { return bbs.back();  }

    /**
     * @brief Returns a constant reference to the last basic block in the function.
     * 
     * @return Constant reference to the last basic block.
     */
    const BB& back() const { return bbs.back();  }

    /**
     * @brief Returns the return type of the function.
     * 
     * @return The return type of the function.
     */
    Type* get_return_type() const noexcept { return get_value_type()->return_ty; }
    
    /**
     * @brief Returns the number of parameters of the function.
     * 
     * @return The number of parameters.
     */
    param_list::size_type param_size() const noexcept { return params.size(); }

    /**
     * @brief Returns an iterator pointing to the first parameter in the function.
     * 
     * @return Iterator pointing to the first parameter.
     */
    param_iterator param_begin() noexcept { return make_indirect_iterator(params.begin()); }

    /**
     * @brief Returns an iterator pointing to the end of the parameters list in the function.
     * 
     * @return Iterator pointing to the end of the parameters list.
     */
    param_iterator param_end() noexcept { return make_indirect_iterator(params.end()); }

    /**
     * @brief Returns a const iterator pointing to the first parameter in the function.
     * 
     * @return Const iterator pointing to the first parameter.
     */
    const_param_iterator param_begin() const noexcept { return make_indirect_iterator(params.begin()); }

    /**
     * @brief Returns a const iterator pointing to the end of the parameters list in the function.
     * 
     * @return Const iterator pointing to the end of the parameters list.
     */
    const_param_iterator param_end() const noexcept { return make_indirect_iterator(params.end()); }

    /// @brief Drop all references within this function.
    void drop_all_references();

    /// @brief Erases this function from its parent module.
    /// @return An iterator pointing to the element after the erased one.
    symbol_table_list<Function>::iterator erase_from_parent();

    /**
     * @brief Checks if the given \c Value is a \c Function.
     * 
     * @param v The \c Value to be checked.
     * @return true if the Value is a \c Function, false otherwise.
     */
    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::FUNCTION;
    }
};


#endif /* PCC_IR_CORE_FUNCTION_H */
