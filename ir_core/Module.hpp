#ifndef PCC_IR_CORE_MODULE_H
#define PCC_IR_CORE_MODULE_H

#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "symbol_table_list.hpp"


class IRContext;


/**
 * @class Module
 * @brief Represents a module in the Intermediate Representation (IR). 
 *
 * A module can contain multiple global variables and functions.
 */
class Module
{
    friend class GlobalVariable;
    friend class Function;

public:
    using global_list = symbol_table_list<GlobalVariable>;
    using function_list = symbol_table_list<Function>;
    using symbol_table = global_list::symbol_table;
    using iterator = function_list::iterator;
    using const_iterator = function_list::const_iterator;
    using global_iterator = global_list::iterator;
    using const_global_iterator = global_list::const_iterator;
    using size_type = function_list::size_type;
    using global_size_type = global_list::size_type;

private:
    IRContext& context;         ///< Reference to the context in which this module exists.
    global_list gvars;          ///< List of global variables in this module.
    function_list functions;    ///< List of functions in this module.
    symbol_table sym_tab;       ///< Symbol table for efficient lookup of global variables and functions.

    global_list& get_global_list() { return gvars; }
    const global_list& get_global_list() const { return gvars; }

    function_list& get_function_list() { return functions; }
    const function_list& get_function_list() const { return functions; }

public:
    Module() = delete;
    Module(IRContext& context);
    Module(const Module&) = delete;
    Module& operator=(const Module&) = delete;
    ~Module();

    // Various iterator functions similar to STL containers for functions.
    iterator begin() noexcept { return functions.begin(); }
    const_iterator begin() const noexcept { return functions.begin(); }
    iterator end() noexcept { return functions.end(); }
    const_iterator end() const noexcept { return functions.end(); }
    
    size_type size() const noexcept { return functions.size(); }
    bool empty() const noexcept { return functions.empty(); }

    // Various iterator functions similar to STL containers for global variables.
    global_iterator global_begin() noexcept { return gvars.begin(); }
    const_global_iterator global_begin() const noexcept { return gvars.begin(); }
    global_iterator global_end() noexcept { return gvars.end(); }
    const_global_iterator global_end() const noexcept { return gvars.end(); }
    
    global_size_type global_size() const noexcept { return gvars.size(); }
    bool global_empty() const noexcept { return gvars.empty(); }

    /**
     * @brief Retrieves the global variable with the given type and name if it exists; otherwise, inserts a new one.
     * 
     * @param ty The type of the global variable.
     * @param name The name of the global variable.
     * 
     * @return Pointer to the retrieved or inserted global variable.
     */
    GlobalVariable* get_or_insert_global(Type* ty, const std::string& name);

    /**
     * @brief Retrieves the function with the given type and name if it exists; otherwise, inserts a new one.
     * 
     * @param ty The type of the function.
     * @param name The name of the function.
     * 
     * @return Pointer to the retrieved or inserted function.
     */
    Function* get_or_insert_funtion(Type* ty, const std::string& name);

    /**
     * @brief Retrieves the global variable with the given name.
     * 
     * @param name The name of the global variable.
     * 
     * @return Pointer to the retrieved global variable, or nullptr if not found.
     */
    GlobalVariable *get_global(const std::string& name);

    /**
     * @brief Retrieves the function with the given name.
     * 
     * @param name The name of the function.
     * 
     * @return Pointer to the retrieved function, or nullptr if not found.
     */
    Function *get_function(const std::string& name);

    /**
     * @brief Get the context in which this module exists.
     * 
     * @return Reference to the IRContext.
     */
    IRContext &get_context() const { return context; }

    /**
     * @brief Prints the representation of the module to the given output stream.
     * 
     * @param os The output stream to print to.
     * @param debug A flag indicating whether to print debug information.
     */
    void print(std::ostream& os, bool debug = false) const;

    /**
     * @brief Overloaded operator for printing the module to an output stream.
     * 
     * @param os The output stream.
     * @param module The module to be printed.
     * 
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Module& module);

};




#endif /* PCC_IR_CORE_MODULE_H */
