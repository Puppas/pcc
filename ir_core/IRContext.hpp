#ifndef PCC_IR_CORE_IRCONTEXT_H
#define PCC_IR_CORE_IRCONTEXT_H


#include <unordered_map>
#include <string>
#include <unordered_set>
#include "Module.hpp"



/**
 * @class IRContext
 * @brief Represents the context for the Intermediate Representation (IR).
 * 
 * The IRContext manages modules, constant integers, and value names within the IR.
 */
class IRContext
{
    friend class ConstantInt;
    friend class GlobalObject;
    friend class Module;

private:
    std::unordered_set<Module*> modules;
    std::unordered_map<std::int64_t, ConstantInt*> int_constants;
    std::unordered_map<const Value*, std::string> value_names;

    /**
     * @brief Retrieves the constant integer with the given value if it exists; otherwise, creates a new one.
     * 
     * @param val The value of the constant integer.
     * @return Pointer to the constant integer.
     */
    ConstantInt* get_constant(std::int64_t val);

    std::string get_name(const Value* val) {
        return value_names[val];
    }

    void set_name(const Value *val, const std::string& name) {
        value_names[val] = name;
    }

    void delete_name(const Value* val) {
        value_names.erase(val);
    }

    void add_module(Module* m) {
        modules.insert(m);
    }

    void remove_module(Module* m) {
        modules.erase(m);
    }

public:
    IRContext() = default;
    IRContext(const IRContext&) = delete;
    IRContext& operator=(const IRContext&) = delete;
    ~IRContext();
};


#endif /* PCC_IR_CORE_IRCONTEXT_H */
