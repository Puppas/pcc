#ifndef PCC_IR_CORE_USE_H
#define PCC_IR_CORE_USE_H


#include "Value.hpp"



/**
 * @class Use
 * @brief A class that represents the use of a \c Value.
 *
 * This class links a \c Value to its \c User.
 */
class Use
{
    friend class User;
private:
    User* user;
    Value* val;

    Use() = delete;

    /**
     * @brief Construct a new \c Use object.
     * 
     * The constructor automatically adds \p user to the list of users of \p val.
     *
     * @param user The user of the value.
     * @param val The value being used.
     */
    Use(User* user, Value* val): user(user), val(val) {
        val->add_user(user);
    }

    Use(const Use&) = delete;
    Use& operator=(const Use&) = delete;

    ~Use() {
        if (val)
            val->remove_user(user);
    }

public:

    /**
     * @brief \c Use can be converted to \c Value*
     * 
     * @return The \c Value being used.
     */
    operator Value* () const noexcept { return val; }

    /**
     * @brief Get the \c Value being used.
     *
     * @return The \c Value being used.
     */
    Value *get() const noexcept { return val; }

    /**
     * @brief Get the \c User of the \c Value.
     *
     * @return The \c User of the \c Value.
     */
    User *get_user() const noexcept { return user; };

    /**
     * @brief Set the \c Value to be used.
     *
     * @param v The new \c Value to be used.
     */
    void set(Value *v) {
        if (val)
            val->remove_user(user);
        if (v)
            v->add_user(user);
        val = v;
    }


    /**
     * @brief Assignment operator for assigning a new \c Value to be used.
     *
     * @param v The new \c Value to be used.
     * @return Reference to the \c Use object.
     */
    Use& operator=(Value* v) {
        set(v);
        return *this;
    }

    /**
     * @brief Overloaded arrow operator to directly access the \c Value.
     *
     * @return Pointer to the \c Value.
     */
    Value *operator->() noexcept { return val; }

    /**
     * @brief Overloaded arrow operator to directly access the \c Value (const version).
     *
     * @return Pointer to the const \c Value.
     */
    const Value *operator->() const noexcept { return val; }
};


#endif /* PCC_IR_CORE_USE_H */
