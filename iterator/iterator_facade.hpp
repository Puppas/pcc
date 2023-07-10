#ifndef PCC_ITERATOR_ITERATOR_FACADE_H
#define PCC_ITERATOR_ITERATOR_FACADE_H

#include <iterator>
#include "iterator_facade_base.hpp"



template <typename Derived,             // The derived iterator type being constructed
          typename Value, typename Category, 
          typename Reference = Value&, typename Difference = std::ptrdiff_t>
class iterator_facade : public iterator_facade_base<Derived, Value,
                            Category, Reference, Difference,
                            std::is_convertible_v< Category, std::bidirectional_iterator_tag >,
                            std::is_convertible_v< Category, std::random_access_iterator_tag >>
{
protected:
    // For use by derived classes
    using iterator_facade_ = iterator_facade<Derived, Value, Category, Reference, Difference>;
};



// Comparison operators
template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline enable_if_interoperable_t<Derived1, Derived2, bool>
operator ==(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs)
{
    return iterator_core_access::equal(*static_cast<Derived1 const*>(&lhs),
                                       *static_cast<Derived2 const*>(&rhs));
}

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline enable_if_interoperable_t<Derived1, Derived2, bool>
operator !=(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs)
{
    return !(lhs == rhs);
}

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline enable_if_interoperable_t<Derived1, Derived2, bool>
operator <(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
           iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs)
{
    return 0 < iterator_core_access::distance_to(*static_cast<Derived1 const*>(&lhs), 
                                                 *static_cast<Derived2 const*>(&rhs));
}

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline enable_if_interoperable_t<Derived1, Derived2, bool>
operator >(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
           iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs)
{
    return 0 > iterator_core_access::distance_to(*static_cast<Derived1 const*>(&lhs), 
                                                 *static_cast<Derived2 const*>(&rhs));
}

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline enable_if_interoperable_t<Derived1, Derived2, bool>
operator <=(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs)
{
    return !(lhs > rhs);
}

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline enable_if_interoperable_t<Derived1, Derived2, bool>
operator >=(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs)
{
    return !(lhs < rhs);
}


// Iterator difference
template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline decide_facade_diff_t<iterator_facade<Derived1,Value1,Category1,Reference1,Difference1>,
                            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2>>
operator-(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
          iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs)
{
    return iterator_core_access::distance_to(*static_cast<Derived2 const*>(&rhs),
                                             *static_cast<Derived1 const*>(&lhs));
}

// Iterator addition
template <typename Derived, typename Value, typename Category, typename Reference, typename Difference>
inline Derived operator+ (iterator_facade<Derived,Value,Category,Reference,Difference> const& i, 
                          typename Derived::difference_type n)
{
    Derived tmp(static_cast<Derived const&>(i));
    return tmp += n;
}

template <typename Derived, typename Value, typename Category, typename Reference, typename Difference>
inline Derived operator+ (typename Derived::difference_type n, 
        iterator_facade<Derived,Value,Category,Reference,Difference> const& i)
{
    return i + n;
}



#endif /* PCC_ITERATOR_ITERATOR_FACADE_H */
