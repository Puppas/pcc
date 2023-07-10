#ifndef PCC_IR_CORE_IRBUILDER_H
#define PCC_IR_CORE_IRBUILDER_H


#include "IRContext.hpp"
#include "iterator/to_address.hpp"


/**
 * @class IRBuilder
 * @brief Utility class for building instructions in an \c IRContext.
 *
 * The \c IRBuilder class provides a convenient interface for creating and inserting instructions
 * into basic blocks within an IRContext. It keeps track of the current parent basic block and
 * the insertion point within that block.
 */
class IRBuilder {
private:
    IRContext& context;
    BB* parent;
    BB::iterator insert_point;

public:
    IRBuilder() = delete;

    /**
     * @brief Constructs an \c IRBuilder object with the specified \c IRContext and parent basic block.
     * @param context The \c IRContext associated with the builder.
     * @param parent The parent basic block.
     */
    IRBuilder(IRContext& context, BB* parent) : 
        context(context), parent(parent), insert_point(parent->end()) {}

    /**
     * @brief Constructs an \c IRBuilder object with the given \c IRContext and insertion point.
     * @param context The \c IRContext associated with the builder.
     * @param inst The insertion point instruction
     */
    IRBuilder(IRContext& context, Inst* inst) : 
        context(context), parent(inst->get_parent()), insert_point(inst) {}

    IRBuilder(const IRBuilder&) = delete;
    IRBuilder& operator=(const IRBuilder&) = delete;

    /**
     * @brief Retrieves the parent basic block where instructions are being inserted.
     * @return The parent basic block
     */
    BB *get_insert_block() const { return parent; }

    /**
     * @brief Retrieves the insertion point iterator within the basic block.
     * @return The insertion point iterator
     */
    BB::iterator get_insert_point() const noexcept { return insert_point; }

    /**
     * @brief Sets the insertion point to the end of a basic block.
     * @param block The basic block to set as the insertion point
     */
    void set_insert_point(BB* block) {
        parent = block;
        insert_point = block->end();
    }

    /**
     * @brief Sets the insertion point to the given instruction.
     * @param inst The instruction to set as the insertion point
     */
    void set_insert_point(Inst* inst) {
        parent = inst->get_parent();
        insert_point = inst;
    }

    /**
     * @brief Creates a constant integer value.
     * @param val The integer value
     * @return Pointer to the created \c ConstantInt object
     */
    ConstantInt *get_int(std::int64_t val) {
        return ConstantInt::get(context, val);
    }

    /**
     * @brief Creates a unary instruction.
     * @param kind The kind of unary instruction
     * @param src The source value
     * @return Pointer to the created \c UnaryInst object
     */
    UnaryInst* create_unary(ValueKind kind, Value* src) {
        return new UnaryInst(kind, src, parent, to_address(insert_point));
    }

    /**
     * @brief Creates a binary instruction.
     * @param kind The kind of binary instruction
     * @param lhs The left-hand side value
     * @param rhs The right-hand side value
     * @return Pointer to the created \c BinaryInst object
     */
    BinaryInst* create_binary(ValueKind kind, Value* lhs, Value* rhs) {
        return new BinaryInst(kind, lhs, rhs, parent, to_address(insert_point));
    }

    /**
     * @brief Creates a load instruction.
     * @param src The source value to load from
     * @return Pointer to the created \c LoadInst object
     */
    LoadInst* create_load(Value* src) {
        return new LoadInst(src, parent, to_address(insert_point));
    }

    /**
     * @brief Creates a cast instruction.
     * @param ty The type to cast to
     * @param src The source value to cast
     * @return Pointer to the created \c CastInst object
     */
    CastInst* create_cast(Type* ty, Value* src) {
        return new CastInst(ty, src, parent, to_address(insert_point));
    }

    /**
     * @brief Creates a store instruction.
     * @param src The source value to store
     * @param dst The destination value to store to
     * @return Pointer to the created \c StoreInst object
     */
    StoreInst* create_store(Value* src, Value* dst) {
        return new StoreInst(src, dst, parent, to_address(insert_point));
    }

    /**
     * @brief Creates a comparison instruction.
     * @param kind The kind of comparison instruction
     * @param lhs The left-hand side value
     * @param rhs The right-hand side value
     * @return Pointer to the created \c CmpInst object
     */
    CmpInst* create_cmp(ValueKind kind, Value* lhs, Value* rhs) {
        return new CmpInst(kind, lhs, rhs, parent, to_address(insert_point));
    }

    /**
     * @brief Creates a return instruction.
     * @param ret The return value
     * @return Pointer to the created \c RetInst object
     */
    RetInst* create_ret(Value* ret) {
        return new RetInst(ret, parent, to_address(insert_point));
    }

    /**
     * @brief Creates an alloca instruction.
     * @param ty The type of the allocated memory
     * @return Pointer to the created \c AllocaInst object
     */
    AllocaInst* create_alloca(Type* ty) {
        return new AllocaInst(ty, parent, to_address(insert_point));
    }

    /**
     * @brief Creates a branch instruction to the given destination basic block.
     * @param dst The destination basic block
     * @param args The arguments for the branch instruction (optional)
     * @return Pointer to the created \c BrInst object
     */
    BrInst* create_br(BB* dst, const std::vector<Value*>& args = {}) {
        return new BrInst(dst, parent, to_address(insert_point), args);
    }

    /**
     * @brief Creates a conditional branch instruction.
     * @param cond The condition value
     * @param then_ The "then" basic block
     * @param else_ The "else" basic block
     * @param then_args The arguments for the "then" basic block (optional)
     * @param else_args The arguments for the "else" basic block (optional)
     * @return Pointer to the created \c BrInst object
     */
    BrInst* create_cond_br(Value* cond, BB* then_, BB* else_, 
        const std::vector<Value*>& then_args = {}, 
        const std::vector<Value*>& else_args = {}) 
    {
        return new BrInst(cond, then_, else_, parent, to_address(insert_point), then_args, else_args);
    }


    /**
     * @brief Creates a function call instruction.
     * @param callee The callee function
     * @param args The arguments for the function call
     * @return Pointer to the created \c CallInst object
     */
    CallInst* create_call(Function* callee, const std::vector<Value*>& args) {
        return new CallInst(callee, args, parent, to_address(insert_point));
    }
    
};



#endif /* PCC_IR_CORE_IRBUILDER_H */
