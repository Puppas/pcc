#ifndef PCC_IR_CORE_DOMINATORS_H
#define PCC_IR_CORE_DOMINATORS_H


#include "Function.hpp"


/**
 * @class DomTreeNode
 * @brief Represents a node in the dominator tree.
 * 
 * Each node in the tree corresponds to a basic block within the function.
 * The tree structure encodes the dominator-subordinate relationships between basic blocks.
 */
class DomTreeNode
{
    friend class DominatorTree;

private:
    Function::size_type num;    ///< The order of this node in dfs
    BB* block;  ///< The basic block corresponding to this node.
    DomTreeNode* idom;  ///< The immediate dominator of this node.
    std::vector<DomTreeNode*> children;  ///< The child nodes of this node (those it dominates).

    DomTreeNode(Function::size_type n, BB* bb):
        num(n), block(bb), idom(nullptr) {}

public:
    /**
     * @brief Returns the basic block corresponding to this node.
     * @return The basic block corresponding to this node.
     */
    BB* get_block() const noexcept { return block; }

    /**
     * @brief Returns the immediate dominator of this node.
     * @return The immediate dominator of this node.
     */
    DomTreeNode* get_idom() const noexcept { return idom; }

    /**
     * @brief Returns the child nodes of this node.
     * @return The child nodes of this node.
     */
    auto get_children() const noexcept { 
        return make_range(make_indirect_iterator(children.begin()), 
                          make_indirect_iterator(children.end())); 
    }
};


/**
 * @class DominatorTree
 * @brief Represents the dominator tree of a function.
 * 
 * A dominator tree is a tree where each node corresponds to a basic block, 
 * and each node's children are those blocks it immediately dominates.
 */
class DominatorTree 
{
private:
    std::unordered_map<BB*, DomTreeNode*> doms;
    DomTreeNode* entry;

public:
    DominatorTree() = default;

    /**
     * @brief Construct a new \c DominatorTree object for the given function.
     * @param func The function to construct the dominator tree for.
     */
    DominatorTree(Function* func) {
        recalculate(func);
    }

    ~DominatorTree();

    /**
     * @brief Recalculates the dominator tree for the given function.
     * @param func The function to recalculate the dominator tree for.
     */
    void recalculate(Function* func);

    /**
     * @brief Returns the dominator tree node corresponding to the given basic block.
     * @param block The basic block to get the dominator tree node for.
     * @return The dominator tree node corresponding to the given basic block.
     */
    DomTreeNode* get_node(const BB* block) const noexcept;
    

    /**
     * @brief Returns the root node of the dominator tree.
     * @return The root node of the dominator tree.
     */
    DomTreeNode* get_root() const noexcept { return entry; }

private:
    DomTreeNode* intersect(DomTreeNode* lhs, DomTreeNode* rhs);
};






#endif /* PCC_IR_CORE_DOMINATORS_H */
