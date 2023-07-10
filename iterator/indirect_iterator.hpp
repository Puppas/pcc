#ifndef PCC_ITERATOR_INDIRECT_ITERATOR_H
#define PCC_ITERATOR_INDIRECT_ITERATOR_H

#include "indirect_iterator_base.hpp"




/**
 * @class indirect_iterator
 * @brief An iterator adapter that allows for indirect iteration.
 *
 * The \c indirect_iterator provides indirect access to the elements
 * the underlying iterator points to. This can be useful in situations where
 * the iterator is over pointers or other iterators.
 *
 * @tparam Iterator The type of the underlying iterator.
 * @tparam Value The value type that the iterator refers to. 
 * @tparam Category The category of the iterator. 
 * @tparam Reference The reference type that the iterator refers to.
 * @tparam Difference The type used to represent the difference between two iterators. 
 */
template <typename Iterator, typename Value = use_default, 
          typename Category = use_default, 
          typename Reference = use_default, 
          typename Difference = use_default>
class indirect_iterator: public indirect_iterator_base<
    indirect_iterator<Iterator, Value, Category, Reference, Difference>, 
    Iterator, Value, Category, Reference, Difference>::type
{
    friend class iterator_core_access;

private:
    using super_t = typename indirect_iterator_base<
        indirect_iterator<Iterator, Value, Category, Reference, Difference>, 
        Iterator, Value, Category, Reference, Difference>::type;

public:
    using reference = typename super_t::reference;

    /// @brief Default constructor.
    indirect_iterator() = default;

    /**
     * @brief Construct from base iterator.
     *
     * @param iter The base iterator to construct from.
     */
    explicit indirect_iterator(Iterator iter): super_t(iter) {}

    /**
     * @brief Copy constructor.
     *
     * @param other The other indirect_iterator to copy from.
     */
    template <typename Iterator2, typename Value2, typename Category2, 
              typename Reference2, typename Difference2>
    indirect_iterator(indirect_iterator<Iterator2, Value2, Category2, Reference2, Difference2> const& other, 
        enable_if_convertible_t<Iterator2, Iterator>* = nullptr): super_t(other.base()) {}


    /**
     * @brief Assignment operator.
     *
     * @param other The other indirect_iterator to assign from.
     * @return Reference to the assigned object.
     */
    template <typename Iterator2, typename Value2, typename Category2, 
              typename Reference2, typename Difference2,
              typename = enable_if_convertible_t<Iterator2, Iterator>>
    indirect_iterator& operator=(indirect_iterator<
        Iterator2, Value2, Category2, Reference2, Difference2> const& other) 
    {
        this->base_reference() = other.base();
        return *this;
    }

private:
    reference dereference() const {
        return **(this->base());
    }
};



/**
 * @brief Helper function to create an indirect iterator.
 *
 * @tparam Iterator The type of the underlying iterator.
 * @param x The iterator to adapt into an indirect iterator.
 * @return An indirect_iterator adapted from x.
 */
template<typename Iterator>
inline indirect_iterator<Iterator> make_indirect_iterator(Iterator x)
{
    return indirect_iterator<Iterator>(x);
}


#endif /* PCC_ITERATOR_INDIRECT_ITERATOR_H */
