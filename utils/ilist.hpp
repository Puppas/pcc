#ifndef PCC_UTILS_ILIST_H
#define PCC_UTILS_ILIST_H

#include <memory>
#include "iterator/iterator_adaptor.hpp"
#include "type_traits.hpp"
#include "iterator/to_address.hpp"


template <typename T>
class ilist;


/**
 * @class ilist_node
 * @brief Node for the intrusive list.
 * 
 * A node class for ilist. Friend of class ilist.
 * 
 * @tparam Derived Data type of the node.
 */
template <typename Derived>
class ilist_node {
    friend class ilist<Derived>;
    Derived* next;
    Derived* prev;
};


/**
 * @class ilist
 * @brief An intrusive doubly-linked list.
 * 
 * @tparam T Data type of the list.
 */
template <typename T>
class ilist {
private:
    template<typename U>
    class ilist_iterator: public iterator_adaptor<ilist_iterator<U>, U*, U, std::bidirectional_iterator_tag> {
    private:
        friend class iterator_core_access;
        using super_t = iterator_adaptor<ilist_iterator<U>, U*, U, std::bidirectional_iterator_tag>;

    public:
        using pointer = typename super_t::pointer;

        ilist_iterator() = default;
        ilist_iterator(pointer node) : super_t(node) {}

        template<typename OtherU>
        ilist_iterator(const ilist_iterator<OtherU>& other): super_t(other.base()) {}

        template<typename OtherU> 
        ilist_iterator& operator=(const ilist_iterator<OtherU>& other) {
            this->base_reference() = other.base();
            return *this;
        }
    
    private:
        void increment() { this->base_reference() = this->base()->next; }
        void decrement() { this->base_reference() = this->base()->prev; }
    };
    

public:
    using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
	using size_type = std::size_t;
	using iterator = ilist_iterator<value_type>;
	using const_iterator = ilist_iterator<const value_type>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using difference_type = iterator_difference_t<iterator>;

private:
    pointer blank;
    size_type count;
    std::allocator<T> alloca;

    void delete_node(pointer node) {
        node->~value_type();
        alloca.deallocate(node, 1);
    }

public:
    /**
     * @brief Construct a new ilist object
     * 
     */
    ilist(): blank(alloca.allocate(1)), count(0) {
        blank->next = blank;
        blank->prev = blank;
    }

    ilist(const ilist&) = delete;
    ilist& operator=(const ilist&) = delete;

    /**
     * @brief Destroy the ilist object. Frees all the nodes in the list.
     * 
     */
    ~ilist() {
        while (!empty()) {
            pop_back();  // Remove and deallocate all nodes
        }
        alloca.deallocate(blank, 1);
    }

    iterator begin() noexcept { return iterator(blank->next); }
    iterator end() noexcept { return iterator(blank); }
    
    const_iterator begin() const noexcept { return const_iterator(blank->next); }
    const_iterator end() const noexcept { return const_iterator(blank); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(blank->prev); }
    reverse_iterator rend() noexcept { return reverse_iterator(blank); }

    const_reverse_iterator rbegin() const noexcept { 
        return const_reverse_iterator(blank->prev); 
    }
    const_reverse_iterator rend() const noexcept { 
        return const_reverse_iterator(blank); 
    }

    size_type size() const noexcept { return count; }

    void push_back(pointer node) {
        insert(end(), node);
    }

    void pop_back() {
        erase(std::prev(end()));
    }

    /**
     * @brief Inserts an element at the specified position in the list.
     * @param pos Iterator to the position to insert before.
     * @param node Pointer to the element to insert.
     * @return An iterator to the inserted element.
     */
    iterator insert(iterator pos, pointer node) {
        pointer pos_ = to_address(pos);
        node->next = pos_;
        node->prev = pos_->prev;
        pos_->prev->next = node;
        pos_->prev = node;

        ++count;
        return iterator(node);
    }

    /**
     * @brief Erases an element from the list.
     * @param pos Iterator to the element to erase.
     * @return An iterator to the element following the erased one.
     */
    iterator erase(iterator pos) {
        pointer node = to_address(pos);
        ++pos;

        node->prev->next = node->next;
        node->next->prev = node->prev;
        
        delete_node(node);
        --count;
        return pos;
    }

    reference front() {
        return *(blank->next);
    }

    const_reference front() const {
        return *(blank->next);
    }

    reference back() {
        return *(blank->prev);
    }

    const_reference back() const {
        return *(blank->prev);
    }

    /**
     * @brief Checks if the list is empty.
     * @return true if the list is empty, false otherwise.
     */
    bool empty() const noexcept {
        return count == 0;
    }
};



#endif /* PCC_UTILS_ILIST_H */
