#ifndef PCC_IR_CORE_DOMINATORS_H
#define PCC_IR_CORE_DOMINATORS_H

#include "BasicBlock.hpp"
#include "POTraversal.hpp"
#include "iterator/to_address.hpp"


// node traits to get the parent type
template<typename NodeT>
struct DomTreeNodeTraits
{
    using parent_ptr = decltype(std::declval<NodeT>().get_parent());
    using parent = std::remove_pointer_t<parent_ptr>;
};



template<typename NodeT, bool Post>
class DominatorTreeBase;


/**
 * @class DomTreeNodeBase
 * @brief Represents a node in the dominator tree.
 * 
 * The tree structure encodes the dominator-subordinate relationships between nodes.
 * 
 * @tparam \c NodeT The actual block type.
 */
template<typename NodeT>
class DomTreeNodeBase
{
    template<typename Node, bool Post>
    friend class DominatorTreeBase;

private:
    std::size_t num;    ///< The order of this node in dfs
    NodeT* block;  ///< The basic block corresponding to this node.
    DomTreeNodeBase* idom;  ///< The immediate dominator of this node.
    std::vector<DomTreeNodeBase*> children;  ///< The child nodes of this node (those it dominates).

    DomTreeNodeBase(std::size_t n, NodeT* bb):
        num(n), block(bb), idom(nullptr) {}

public:
    /**
     * @brief Returns the basic block corresponding to this node.
     * @return The basic block corresponding to this node.
     */
    NodeT* get_block() const noexcept { return block; }

    /**
     * @brief Returns the immediate dominator of this node.
     * @return The immediate dominator of this node.
     */
    DomTreeNodeBase* get_idom() const noexcept { return idom; }

    /**
     * @brief Returns the child nodes of this node.
     * @return The child nodes of this node.
     */
    auto get_children() const noexcept { 
        return make_range(make_indirect_iterator(children.begin()), 
                          make_indirect_iterator(children.end())); 
    }
};


using DomTreeNode = DomTreeNodeBase<BB>;


/**
 * @class DominatorTreeBase
 * @brief Represents the dominator tree of a graph.
 * 
 * This class is a generic template over graph nodes. It is instantiated for
 * various graphs in the LLVM IR or in the code generator.
 * 
 * @tparam \c NodeT The block type.
 * @tparam \c Post Indicate whether it's a postdominator tree.
 */
template<typename NodeT, bool Post>
class DominatorTreeBase
{
private:
    std::unordered_map<NodeT*, DomTreeNodeBase<NodeT>*> doms;
    DomTreeNodeBase<NodeT>* entry;
    static constexpr bool IsPostDominator = Post;

public:
    using node_traits = DomTreeNodeTraits<NodeT>;
    using parent_ptr = typename node_traits::parent_ptr;
    using parent = typename node_traits::parent;


    DominatorTreeBase() = default;

    /**
     * @brief Construct a new \c DominatorTreeBase object for the given function.
     * @param func The function to construct the dominator tree for.
     */
    DominatorTreeBase(parent_ptr func) {
        recalculate(func);
    }

    ~DominatorTreeBase();

    /**
     * @brief Recalculates the dominator tree for the given function.
     * @param func The function to recalculate the dominator tree for.
     */
    void recalculate(parent_ptr func);

    /**
     * @brief Returns the dominator tree node corresponding to the given basic block.
     * @param block The basic block to get the dominator tree node for.
     * @return The dominator tree node corresponding to the given basic block.
     */
    DomTreeNodeBase<NodeT>* get_node(const NodeT* block) const noexcept;

    /**
     * @brief Returns the root node of the dominator tree.
     * @return The root node of the dominator tree.
     */
    DomTreeNodeBase<NodeT>* get_root() const noexcept { return entry; }

private:
    DomTreeNodeBase<NodeT>* intersect(DomTreeNodeBase<NodeT>* lhs, DomTreeNodeBase<NodeT>* rhs);
};


using DominatorTree = DominatorTreeBase<BB, false>;
using PostDominatorTree = DominatorTreeBase<BB, true>;



template<typename NodeT, bool Post>
DomTreeNodeBase<NodeT>* 
DominatorTreeBase<NodeT, Post>::get_node(const NodeT* block) const noexcept 
{ 
    auto iter = doms.find(const_cast<NodeT*>(block));
    if (iter != doms.end()) {
        return iter->second;
    }

    return nullptr;
}


template<typename NodeT, bool Post>
DomTreeNodeBase<NodeT>* 
DominatorTreeBase<NodeT, Post>::intersect(DomTreeNodeBase<NodeT>* lhs, DomTreeNodeBase<NodeT>* rhs) 
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


template<typename NodeT, bool Post>
DominatorTreeBase<NodeT, Post>::~DominatorTreeBase()
{
    for (auto iter = doms.begin(); iter != doms.end(); ++iter) {
        delete iter->second;
    }
}


template<typename NodeT, bool Post>
void DominatorTreeBase<NodeT, Post>::recalculate(parent_ptr func) 
{
    using GT = std::conditional_t<IsPostDominator, 
                    InverseGraphTraits<parent>, 
                    GraphTraits<parent>>;

    int i = 0;
    POTraversal<parent, GT> traversal(func);
    for (auto iter = traversal.begin(); iter != traversal.end(); ++iter) {
        doms[to_address(iter)] = new DomTreeNodeBase<NodeT>(i, to_address(iter));
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
            DomTreeNodeBase<NodeT>* new_idom = nullptr;
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
        DomTreeNodeBase<NodeT>* node = iter->second;
        if (node != entry)
            node->idom->children.push_back(node);
    }
}



#endif /* PCC_IR_CORE_DOMINATORS_H */
