#ifndef PCC_IR_CORE_RPOTRAVERSAL_H
#define PCC_IR_CORE_RPOTRAVERSAL_H

#include "Function.hpp"


/**
 * @class RPOTraversal
 * @brief Class for Reverse Post Order Traversal over a function's basic blocks.
 */
class RPOTraversal
{
private:
    std::unordered_set<BB*> visited; ///< Set of visited basic blocks.
    std::vector<BB*> bbs; ///< basic blocks in post-order.

    void dfs(BB* bb) {
        visited.insert(bb);
        for (auto&& succ : bb->successors()) {
            if (visited.find(&succ) == visited.end())
                dfs(&succ);
        }

        bbs.push_back(bb);
    }

public:

    using iterator = indirect_iterator<std::reverse_iterator<std::vector<BB*>::iterator>>;
    using const_iterator = indirect_iterator<std::reverse_iterator<std::vector<BB*>::const_iterator>>;

    /**
     * @brief Constructs the \c RPOTraversal by performing a DFS on the function's basic blocks.
     * @param fn The function whose basic blocks will be traversed.
     */
    explicit RPOTraversal(const Function* fn) {
        dfs(const_cast<BB*>(&fn->front()));
        visited.clear();
    }

    ~RPOTraversal() = default;

    // Because we want a reverse post order, use reverse iterators from the vector
    iterator begin() { return make_indirect_iterator(bbs.rbegin()); }
    const_iterator begin() const { return make_indirect_iterator(bbs.rbegin()); }

    iterator end() { return make_indirect_iterator(bbs.rend()); }
    const_iterator end() const { return make_indirect_iterator(bbs.rend()); }
};


#endif /* PCC_IR_CORE_RPOTRAVERSAL_H */
