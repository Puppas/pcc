#include "Dominators.hpp"
#include "RPOTraversal.hpp"
#include "iterator/to_address.hpp"



DominatorTree::~DominatorTree()
{
    for (auto iter = doms.begin(); iter != doms.end(); ++iter) {
        delete iter->second;
    }
}


void DominatorTree::recalculate(Function* func) 
{
    auto n = func->size();
    auto i = 0;
    RPOTraversal traversal(func);
    for (auto iter = traversal.begin(); iter != traversal.end(); ++iter) {
        doms[to_address(iter)] = new DomTreeNode(n - i - 1, to_address(iter));
        ++i;
    }

    entry = doms[&func->front()];
    entry->idom = entry;

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto bb = std::next(traversal.begin()); bb != traversal.end(); ++bb) 
        {
            DomTreeNode* new_idom = nullptr;
            bool first = true;
            for (auto&& pred: bb->predecessors()) {
                if (doms[&pred]->idom) {
                    if (first) {
                        new_idom = doms[&pred];
                        first = false;
                    }
                    else
                        new_idom = intersect(new_idom, doms[&pred]);
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


DomTreeNode* DominatorTree::get_node(const BB* block) const noexcept 
{ 
    auto iter = doms.find(const_cast<BB*>(block));
    if (iter != doms.end()) {
        return iter->second;
    }

    return nullptr;
}


DomTreeNode* DominatorTree::intersect(DomTreeNode* lhs, DomTreeNode* rhs) 
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