#ifndef PCC_IR_CORE_GLOBALOBJECT_H
#define PCC_IR_CORE_GLOBALOBJECT_H

#include <string>
#include "Constant.hpp"

class Module;
class IRContext;


/**
 * @class GlobalObject
 * @brief This class represents a global object in the intermediate representation.
 * 
 * Global objects are entities whose lifetime extends across the entire run of the program. 
 * They include global variables and functions. This class is designed as a base class for 
 * different types of global objects.
 */
class GlobalObject: public Constant
{
private:
    Type* value_ty;
    Module* parent;

protected:
    /**
     * @brief Constructs a new \c GlobalObject.
     * 
     * @param ty The type of the global object.
     * @param kind The kind of the global object.
     * @param name The name of the global object.
     * @param parent The module in which to create the global object.
     */
    GlobalObject(Type* ty, ValueKind kind, const std::string& name, Module* parent):
        Constant(pointer_to(ty), kind), value_ty(ty), parent(parent) 
    {
        set_name(name);
    }

    GlobalObject(const GlobalObject&) = delete;
    GlobalObject& operator=(const GlobalObject&) = delete;

    ~GlobalObject();

public:
    /**
     * @brief Returns the context in which this global object is defined.
     * 
     * @return The context.
     */
    IRContext& get_context() const;
    Type* get_value_type() const noexcept { return value_ty; }

    /**
     * @brief Returns the module in which this global object is defined.
     * 
     * @return The module.
     */
    Module *get_parent() { return parent; }
    const Module *get_parent() const { return parent; }

    /**
     * @brief Gets the name of the global object.
     * 
     * @return The name of the global object.
     */
    std::string get_name() const;

    /**
     * @brief Sets the name of the global object.
     * 
     * @param name The new name of the global object.
     */
    void set_name(const std::string& name);

    static bool classof(const Value* v) {
        return v->get_kind() > ValueKind::GLOBAL_BEGIN &&
               v->get_kind() < ValueKind::GLOBAL_END;
    }
};



#endif /* PCC_IR_CORE_GLOBALOBJECT_H */
