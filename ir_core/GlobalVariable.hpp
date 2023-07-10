#ifndef PCC_IR_CORE_GLOBALVARIABLE_H
#define PCC_IR_CORE_GLOBALVARIABLE_H

#include <memory>
#include "GlobalObject.hpp"
#include "symbol_table_list.hpp"


/**
 * @class GlobalVariable
 * @brief This class represents a global variable in the intermediate representation.
 * 
 * Global variables are variables that are defined outside any function and can be accessed throughout 
 * the program. They are also part of the global scope.
 */
class GlobalVariable: public GlobalObject, public ilist_node<GlobalVariable>
{
    friend class Module;
    friend class ilist<GlobalVariable>;

private:
    /**
     * @brief Constructs a new \c GlobalVariable.
     * 
     * @param ty The type of the global variable.
     * @param name The name of the global variable.
     * @param parent The module in which to create the global variable.
     */
    GlobalVariable(Type* ty, const std::string& name, Module* parent);
    GlobalVariable(const GlobalVariable&) = delete;
    GlobalVariable& operator=(const GlobalVariable&) = delete;
    ~GlobalVariable() = default;;

public:    
    /// @brief Erases this global variable from its parent module.
    /// @return An iterator pointing to the element after the erased one.
    symbol_table_list<GlobalVariable>::iterator erase_from_parent();

    static bool classof(const Value* v) {
        return v->get_kind() == ValueKind::GLOBAL_VARIABLE;
    }
};



#endif /* PCC_IR_CORE_GLOBALVARIABLE_H */
