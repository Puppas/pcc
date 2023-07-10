#ifndef PCC_ITERATOR_ITERATOR_CORE_ACCESS_H
#define PCC_ITERATOR_ITERATOR_CORE_ACCESS_H


#include "utils/type_traits.hpp"


template <typename Derived, typename Value,
          typename Category, typename Reference, typename Difference,
          bool IsBidirectionalTraversal, bool IsRandomAccessTraversal> 
class iterator_facade_base;

template <typename Derived, typename Value, 
          typename Category, typename Reference, typename Difference>
class iterator_facade;


/// @brief Iterator interface to \c iterator_facade
/// 
/// In order to hide core operations, we should grant friendship to \c iterator_core_access when
/// deriving from \c iterator_facade / \c iterator_adaptor
class iterator_core_access
{   

// iterator_facade can use these definitions
template <typename Derived, typename Value,
          typename Category, typename Reference, typename Difference,
          bool IsBidirectionalTraversal, bool IsRandomAccessTraversal> 
friend class iterator_facade_base;

template <typename Derived, typename Value, 
          typename Category, typename Reference, typename Difference>
friend class iterator_facade;


// iterator comparisons are our friends
template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline friend enable_if_interoperable_t<Derived1, Derived2, bool>
operator ==(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs);

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline friend enable_if_interoperable_t<Derived1, Derived2, bool>
operator !=(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs);

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline friend enable_if_interoperable_t<Derived1, Derived2, bool>
operator <(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
           iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs);

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline friend enable_if_interoperable_t<Derived1, Derived2, bool>
operator >(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
           iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs);

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline friend enable_if_interoperable_t<Derived1, Derived2, bool>
operator <=(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs);

template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline friend enable_if_interoperable_t<Derived1, Derived2, bool>
operator >=(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
            iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs);

// iterator difference is also our friend
template <typename Derived1, typename Value1, typename Category1, typename Reference1, typename Difference1,
          typename Derived2, typename Value2, typename Category2, typename Reference2, typename Difference2>
inline friend decide_facade_diff_t<iterator_facade<Derived1,Value1,Category1,Reference1,Difference1>,
                                   iterator_facade<Derived2,Value2,Category2,Reference2,Difference2>>
operator-(iterator_facade<Derived1,Value1,Category1,Reference1,Difference1> const& lhs,
          iterator_facade<Derived2,Value2,Category2,Reference2,Difference2> const& rhs);



template<typename Iterator>
static typename Iterator::reference dereference(Iterator const& i) {
    return i.dereference();
}   


template <typename Iterator1, typename Iterator2>
static bool equal(Iterator1 const& i1, Iterator2 const& i2)
{
    return equal(i1, i2, typename std::is_convertible<Iterator2, Iterator1>::type());
}


template <typename Iterator1, typename Iterator2>
static bool equal(Iterator1 const& i1, Iterator2 const& i2, std::true_type)
{
    return i1.equal(i2);
}

template <typename Iterator1, typename Iterator2>
static bool equal(Iterator1 const& i1, Iterator2 const& i2, std::false_type)
{
    return i2.equal(i1);
}

// required of forward iterators
template<typename Iterator>
static void increment(Iterator& i) {
    return i.increment();
}


// required of bidirectional iterators
template<typename Iterator>
static void decrement(Iterator& i) {
    return i.decrement();
}


// required of random-access iterators
template<typename Iterator, typename Distance>
static void advance(Iterator& i, Distance n) {
    return i.advance(n);
}


template <typename Iterator1, typename Iterator2>
static decide_facade_diff_t<Iterator1, Iterator2>
distance_to(Iterator1 const& i1, Iterator2 const& i2)
{
    return distance_to(i1, i2, typename std::is_convertible<Iterator2, Iterator1>::type());
}


template <typename Iterator1, typename Iterator2>
static typename Iterator1::difference_type 
distance_to(Iterator1 const& i1, Iterator2 const& i2, std::true_type)
{
    return i1.distance_to(i2);
}

template <typename Iterator1, typename Iterator2>
static typename Iterator2::difference_type 
distance_to(Iterator1 const& i1, Iterator2 const& i2, std::false_type)
{
    return -i2.distance_to(i1);
}


};  // iterator_core_access



#endif /* PCC_ITERATOR_ITERATOR_CORE_ACCESS_H */
