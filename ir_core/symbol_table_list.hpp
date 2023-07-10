#ifndef PCC_IR_CORE_SYMBOL_TABLE_LIST_H
#define PCC_IR_CORE_SYMBOL_TABLE_LIST_H


#include <unordered_map>
#include "utils/ilist.hpp"
#include "GlobalObject.hpp"



/**
 * @class symbol_table_list
 * @brief This template class is a intrusive list that also maintains a symbol table.
 * 
 * The symbol table refers to the \c Module's memeber.
 */
template<typename T>
class symbol_table_list
{
public:
    using base = ilist<T>;
    using symbol_table = std::unordered_map<std::string, Value*>;
    using value_type = typename base::value_type;
	using reference = typename base::reference;
	using const_reference = typename base::const_reference;
    using pointer = typename base::pointer;
    using const_pointer = typename base::const_pointer;
	using size_type = typename base::size_type;
	using iterator = typename base::iterator;
	using const_iterator = typename base::const_iterator;
	using reverse_iterator = typename base::reverse_iterator;
	using const_reverse_iterator = typename base::const_reverse_iterator;
    using difference_type = typename base::difference_type;

private:
    symbol_table* sym_tab;
    base list;

public:
    symbol_table_list(symbol_table* sym_tab): sym_tab(sym_tab) {}
    ~symbol_table_list() = default;

    // Various iterator functions similar to STL containers.
    iterator begin() noexcept { return list.begin(); }
    iterator end() noexcept { return list.end(); }
    const_iterator begin() const noexcept { return list.begin(); }
    const_iterator end() const noexcept { return list.end(); }
    reverse_iterator rbegin() noexcept { return list.rbegin(); }
    reverse_iterator rend() noexcept { return list.rend(); }
    const_reverse_iterator rbegin() const noexcept { return list.rbegin(); }
    const_reverse_iterator rend() const noexcept { return list.rend(); }

    /**
     * @brief Returns the number of elements in the list.
     * 
     * @return Number of elements in the list.
     */
    size_type size() const noexcept { return list.size(); }

    /**
     * @brief Appends the given node to the end of the list.
     * 
     * @param node The node to append.
     */
    void push_back(pointer node) {
        insert(end(), node);
    }

    /**
     * @brief Removes the last node from the list.
     */
    void pop_back() {
        erase(std::prev(end()));
    }

    /**
     * @brief Inserts a node before the specified position.
     * 
     * @param pos Position before which the node is to be inserted.
     * @param node Node to insert.
     * 
     * @return Iterator pointing to the inserted node.
     */
    iterator insert(iterator pos, pointer node) {
        (*sym_tab)[cast<GlobalObject>(node)->get_name()] = node;
        return list.insert(pos, node);
    }


    /**
     * @brief Erases the node at the specified position.
     * 
     * @param pos Position of the node to erase.
     * 
     * @return Iterator pointing to the position immediately following the erased node.
     */
    iterator erase(iterator pos) {
        sym_tab->erase(cast<GlobalObject>(to_address(pos))->get_name());
        return list.erase(pos);
    }

    reference front() {
        return list.front();
    }

    const_reference front() const {
        return list.front();
    }

    reference back() {
        return list.back();
    }

    const_reference back() const {
        return list.back();
    }

    /**
     * @brief Checks whether the list is empty.
     * 
     * @return true if the list is empty, false otherwise.
     */
    bool empty() const noexcept {
        return list.empty();
    }

};


#endif /* PCC_IR_CORE_SYMBOL_TABLE_LIST_H */
