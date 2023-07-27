#ifndef PCC_IR_CORE_DOMINATORS_H
#define PCC_IR_CORE_DOMINATORS_H


#include "Function.hpp"
#include "POTraversal.hpp"
#include "iterator/to_address.hpp"


template<bool Post>
class DominatorTreeBase;


/**
 * @class DomTreeNode
 * @brief Represents a node in the dominator tree.
 * 
 * Each node in the tree corresponds to a basic block within the function.
 * The tree structure encodes the dominator-subordinate relationships between basic blocks.
 */
class DomTreeNode
{
    template<bool Post>
    friend class DominatorTreeBase;

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
 * 
 * @tparam \c Post Indicate whether it's a postdominator tree.
 */
template<bool Post>
class DominatorTreeBase
{
private:
    std::unordered_map<BB*, DomTreeNode*> doms;
    DomTreeNode* entry;
    static constexpr bool IsPostDominator = Post;

public:
    DominatorTreeBase() = default;

    /**
     * @brief Construct a new \c DominatorTreeBase object for the given function.
     * @param func The function to construct the dominator tree for.
     */
    DominatorTreeBase(Function* func) {
        recalculate(func);
    }

    ~DominatorTreeBase();

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



using DominatorTree = DominatorTreeBase<false>;
using PostDominatorTree = DominatorTreeBase<true>;



template<bool Post>
DomTreeNode* DominatorTreeBase<Post>::get_node(const BB* block) const noexcept 
{ 
    auto iter = doms.find(const_cast<BB*>(block));
    if (iter != doms.end()) {
        return iter->second;
    }

    return nullptr;
}


template<bool Post>
DomTreeNode* DominatorTreeBase<Post>::intersect(DomTreeNode* lhs, DomTreeNode* rhs) 
{
    while (lhs != rhs) 
    {
        while (lhs->num < rhs->num)
            lhs = lhs->idom;

        while (rhs->num < lhs->num)
            rhs = rhs->idom;
    }

    return lhs;
}


template<bool Post>
DominatorTreeBase<Post>::~DominatorTreeBase()
{
    for (auto iter = doms.begin(); iter != doms.end(); ++iter) {
        delete iter->second;
    }
}


template<bool Post>
void DominatorTreeBase<Post>::recalculate(Function* func) 
{
    using GT = std::conditional_t<IsPostDominator, 
                    InverseGraphTraits<Function>, 
                    GraphTraits<Function>>;

    int i = 0;
    POTraversal<Function, GT> traversal(func);
    for (auto iter = traversal.begin(); iter != traversal.end(); ++iter) {
        doms[to_address(iter)] = new DomTreeNode(i, to_address(iter));
        ++i;
    }

    entry = doms[GT::get_entry_node(func)];
    entry->idom = entry;

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto bb = std::next(traversal.rbegin()); bb != traversal.rend(); ++bb) 
        {
            DomTreeNode* new_idom = nullptr;
            bool first = true;
            for (auto&& pred = GT::parent_begin(to_address(bb)); pred != GT::parent_end(to_address(bb)); ++pred) {
                if (doms[to_address(pred)]->idom) {
                    if (first) {
                        new_idom = doms[to_address(pred)];
                        first = false;
                    }
                    else
                        new_idom = intersect(new_idom, doms[to_address(pred)]);
                }
            }

            if (new_idom != doms[to_address(bb)]->idom) {
                doms[to_address(bb)]->idom = new_idom;
                changed = true;
            }
        }       
    }

    for (auto iter = doms.begin(); iter != doms.end(); ++iter) {
        DomTreeNode* node = iter->second;
        if (node != entry)
            node->idom->children.push_back(node);
    }
}



#endif /* PCC_IR_CORE_DOMINATORS_H */
