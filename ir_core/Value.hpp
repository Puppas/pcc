#ifndef PCC_IR_CORE_VALUE_H
#define PCC_IR_CORE_VALUE_H


#include <unordered_set>
#include <assert.h>
#include "iterator/indirect_iterator.hpp"
#include "iterator/iterator_range.hpp"
#include "type.hpp"


class User;


/**
 * @enum ValueKind
 * @brief A enum class to categorize the type of \c Value.
 * 
 * It categorizes the Value into multiple kinds such as INST (instructions),
 * CONSTANT, GLOBAL variables and Basic Blocks (BB).
 */
enum class ValueKind
{
    VALUE,

    INST_BEGIN,
    INST_UNARY_BEGIN,
    INST_NEG,
    INST_LOAD,
    INST_CAST,
    INST_BITNOT,
    INST_UNARY_END,

    INST_BINARY_BEGIN,
    INST_ADD,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_EQ,
    INST_NE,
    INST_LE,
    INST_LT,
    INST_BITAND,
    INST_BITOR,
    INST_BITXOR,
    INST_MOD,
    INST_BINARY_END,

    INST_TERM_BEGIN,
    INST_RETURN,
    INST_BR,
    INST_TERM_END,
    
    INST_CALL,
    INST_ALLOCA,
    INST_STORE,
    INST_END,

    CONSTANT_BEGIN,
    CONSTANT_INT,

    GLOBAL_BEGIN,
    GLOBAL_VARIABLE,
    FUNCTION,
    GLOBAL_END,

    CONSTANT_END,

    BB
};


/**
 * @class Value
 * @brief The base class for all values computed by a program.
 *
 * These include constants, instructions, global variables, etc.
 * Every value has a type.
 */
class Value
{
    friend class Use;
    friend class BB;
    friend class Function;

public:
    using user_list = std::unordered_set<User*>;
    using user_iterator = indirect_iterator<typename user_list::iterator, User>;
    using const_user_iterator = indirect_iterator<typename user_list::const_iterator, const User>;
    
private:
    Type* ty;
    ValueKind kind;
    user_list users;

protected:
    Value() = delete;

    /**
     * @brief Constructor.
     *
     * @param ty The type of the \c Value.
     * @param kind The kind of the \c Value.
     */
    Value(Type* ty, ValueKind kind) noexcept: ty(ty), kind(kind) {}

    // Delete copy constructor and assignment operator
    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;

    virtual ~Value() = default;

    void set_type(Type* new_ty) noexcept {
        ty = new_ty;
    }

    void add_user(User* inst) {
        users.insert(inst);
    }

    void remove_user(User* inst) {
        users.erase(inst);
    }

public:

    /**
     * @brief Get the kind of the \c Value.
     *
     * @return The \c ValueKind of the \c Value.
     */
    ValueKind get_kind() const noexcept {
        return kind;
    }

    /**
     * @brief Get the type of the \c Value.
     *
     * @return A pointer to the \c Type.
     */
    Type *get_type() const noexcept {
        return ty;
    }

    /**
     * @brief Get an iterator to the beginning of the users.
     *
     * @return An iterator pointing to the beginning of the users.
     */
    user_iterator user_begin() noexcept {
        return user_iterator(users.begin());
    }

    /**
     * @brief Get an iterator to the end of the users.
     *
     * @return An iterator pointing to the end of the users.
     */
    user_iterator user_end() noexcept {
        return user_iterator(users.end());
    }
                        
    const_user_iterator user_begin() const noexcept {
        return const_user_iterator(users.begin());
    }

    const_user_iterator user_end() const noexcept {
        return const_user_iterator(users.end());
    }

    /**
     * @brief Get a range of all users of this \c Value.
     *
     * @return A range that includes all users of this \c Value.
     */
    iterator_range<user_iterator> get_users() noexcept {
        return make_range(user_begin(), user_end());
    }

    iterator_range<const_user_iterator> get_users() const noexcept {
        return make_range(user_begin(), user_end());
    }

    /**
     * @brief Checks if this \c Value has any users.
     *
     * @return True if this \c Value has users, false otherwise.
     */
    bool user_empty() const noexcept {
        return !users.empty();
    }

    /**
     * @brief Replaces all uses of this \c Value with another \c Value.
     *
     * @param val The new \c Value.
     */
    void replace_all_uses_with(Value* val);
};


/**
 * @brief Check if the given value is of a certain type.
 *
 * @tparam To The target type to check.
 * @tparam From The original type.
 * @param v The value to check.
 * @return True if the value is of the target type, false otherwise.
 */
template <typename To, typename From> 
inline bool isa(From *v) {
    return To::classof(v);
}

template <typename To, typename From> 
inline bool isa(const From& v) {
    return isa<To>(&v);
}


/**
 * @brief Cast the given value to a certain type.
 *
 * Asserts if the value is not of the target type.
 *
 * @tparam To The target type to cast to.
 * @tparam From The original type.
 * @param v The value to cast.
 * @return The value casted to the target type.
 */
template <typename To, typename From> 
inline To *cast(From *v) {
    assert(isa<To>(v) && "Invalid cast!");
    return static_cast<To *>(v);
}

template <typename To, typename From> 
inline To& cast(const From& v) {
    return *cast<To>(&v);
}

template <typename To, typename From> 
inline To& cast(From& v) {
    return *cast<To>(&v);
}


/**
 * @brief Attempt to cast the given value to a certain type.
 *
 * Returns nullptr if the value is not of the target type.
 *
 * @tparam To The target type to cast to.
 * @tparam From The original type.
 * @param v The value to cast.
 * @return The value casted to the target type, or nullptr if the cast is invalid.
 */
template <typename To, typename From> 
inline To *dyn_cast(From *v) {
    return isa<To>(v) ? cast<To>(v) : nullptr;
}

template <typename To, typename From> 
inline To& dyn_cast(const From& v) {
    return *dyn_cast<To>(&v);
}

template <typename To, typename From> 
inline To& dyn_cast(From& v) {
    return *dyn_cast<To>(&v);
}



#endif /* PCC_IR_CORE_VALUE_H */
