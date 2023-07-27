#ifndef PCC_IR_CORE_POTRAVERSAL_H
#define PCC_IR_CORE_POTRAVERSAL_H


#include "Function.hpp"
#include "GraphTraits.hpp"


/**
 * @class POTraversal
 * @brief Class for Post Order Traversal over a graph.
 * 
 * @tparam Graph The type of the grapth to be traversed
 * @tparam GT The trait of the graph
 */
template<typename Graph, typename GT = GraphTraits<Graph>>
class POTraversal
{
public:
    using node_type = typename GT::node_type;
    using iterator = indirect_iterator<typename std::vector<node_type>::iterator>;
    using const_iterator = indirect_iterator<typename std::vector<node_type>::const_iterator>;
    using reverse_iterator = indirect_iterator<typename std::vector<node_type>::reverse_iterator>;
    using const_reverse_iterator = indirect_iterator<typename std::vector<node_type>::const_reverse_iterator>;

private:
    std::unordered_set<node_type> visited; ///< Set of visited nodes.
    std::vector<node_type> nodes; ///< nodes in post-order.

    void dfs(node_type node) {
        visited.insert(node);
        for (auto&& succ = GT::child_begin(node); succ != GT::child_end(node); ++succ) {
            if (visited.find(to_address(succ)) == visited.end())
                dfs(to_address(succ));
        }

        nodes.push_back(node);
    }

public:
    /**
     * @brief Constructs the \c POTraversal by performing a DFS on the graph.
     * @param g The graph will be traversed.
     */
    explicit POTraversal(const Graph* g) {
        dfs(GT::get_entry_node(const_cast<Graph*>(g)));
        visited.clear();
    }

    ~POTraversal() = default;

    iterator begin() { return make_indirect_iterator(nodes.begin()); }
    const_iterator begin() const { return make_indirect_iterator(nodes.begin()); }

    iterator end() { return make_indirect_iterator(nodes.end()); }
    const_iterator end() const { return make_indirect_iterator(nodes.end()); }

    reverse_iterator rbegin() { return make_indirect_iterator(nodes.rbegin()); }
    const_reverse_iterator rbegin() const { return make_indirect_iterator(nodes.rbegin()); }

    reverse_iterator rend() { return make_indirect_iterator(nodes.rend()); }
    const_reverse_iterator rend() const { return make_indirect_iterator(nodes.rend()); }
};



#endif /* PCC_IR_CORE_POTRAVERSAL_H */
