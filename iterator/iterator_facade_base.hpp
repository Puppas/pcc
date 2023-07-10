#ifndef PCC_ITERATOR_ITERATOR_FACADE_BASE_H
#define PCC_ITERATOR_ITERATOR_FACADE_BASE_H

#include "iterator_core_access.hpp"


// iterator_facade can use these definitions
template <typename Derived, typename Value,
          typename Category, typename Reference, typename Difference,
          bool IsBidirectionalTraversal, bool IsRandomAccessTraversal> 
class iterator_facade_base;


// Implementation for forward traversal iterators
template <typename Derived, typename Value, 
          typename Category, typename Reference, typename Difference>
class iterator_facade_base<Derived, Value, Category, Reference, Difference, false, false>
{
public:
    using value_type = std::remove_const_t<Value>;
    using reference = Reference;
    using pointer = std::conditional_t<is_iterator_writable_v<Value, Reference>, value_type*, const value_type*>;
    using difference_type = Difference;
    using iterator_category = Category;

public:
    reference operator*() const
    {
        return iterator_core_access::dereference(this->derived());
    }

    pointer operator->() const {
        return &(this->operator*());
    }

    Derived& operator++()
    {
        iterator_core_access::increment(this->derived());
        return this->derived();
    }

    Derived operator++(int)
    {
        Derived tmp(this->derived());
        ++*this;
        return tmp;
    }

protected:
    Derived& derived()
    {
        return *static_cast<Derived*>(this);
    }

    Derived const& derived() const
    {
        return *static_cast<Derived const*>(this);
    }
};


// Implementation for bidirectional traversal iterators
template <typename Derived, typename Value,
          typename Category, typename Reference, typename Difference>
class iterator_facade_base< Derived, Value, Category, Reference, Difference, true, false > :
    public iterator_facade_base< Derived, Value, Category, Reference, Difference, false, false >
{
public:
    Derived& operator--()
    {
        iterator_core_access::decrement(this->derived());
        return this->derived();
    }

    Derived operator--(int)
    {
        Derived tmp(this->derived());
        --*this;
        return tmp;
    }
};

   
// Implementation for random access traversal iterators
template <typename Derived, typename Value,
          typename Category, typename Reference, typename Difference>
class iterator_facade_base< Derived, Value, Category, Reference, Difference, true, true > :
    public iterator_facade_base< Derived, Value, Category, Reference, Difference, true, false >
{
private:
    using base_type = iterator_facade_base< Derived, Value, Category, Reference, Difference, true, false >;

public:
    using reference = typename base_type::reference;
    using difference_type = typename base_type::difference_type;

public:
    reference operator[](difference_type n) const
    {
        return *(this->operator+(n));
    }

    Derived& operator+=(difference_type n)
    {
        iterator_core_access::advance(this->derived(), n);
        return this->derived();
    }

    Derived operator+(difference_type n) const
    {
        Derived result(this->derived());
        return result += n;
    }
    
    Derived& operator-=(difference_type n)
    {
        iterator_core_access::advance(this->derived(), -n);
        return this->derived();
    }

    Derived operator-(difference_type n) const
    {
        Derived result(this->derived());
        return result -= n;
    }
};




#endif /* PCC_ITERATOR_ITERATOR_FACADE_BASE_H */
