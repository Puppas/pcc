#ifndef PCC_ITERATOR_ITERATOR_RANGE_H
#define PCC_ITERATOR_ITERATOR_RANGE_H


#include "utils/type_traits.hpp"


template<typename Iterator, bool isRandomAccess>
class iterator_range_base;


// Specialization for non-random access iterators
template<typename Iterator>
class iterator_range_base<Iterator, false>
{
protected:
    Iterator begin_iterator, end_iterator;

    iterator_range_base(Iterator begin_iterator, Iterator end_iterator): 
        begin_iterator(std::move(begin_iterator)),
        end_iterator(std::move(end_iterator)) {}

public:
    using iterator = Iterator;
    using value_type = iterator_value_t<Iterator>;
    using reference = iterator_reference_t<Iterator>;
    using difference_type = iterator_difference_t<Iterator>;
    using size_type = difference_type;


    Iterator begin() const noexcept { return begin_iterator; }
    Iterator end() const noexcept { return end_iterator; }
    bool empty() const noexcept { return begin_iterator == end_iterator; }
};


// Specialization for random access iterators
template<typename Iterator>
class iterator_range_base<Iterator, true>: public iterator_range_base<Iterator, false>
{
private: 
    using super_t = iterator_range_base<Iterator, false>;
    using size_type = typename super_t::size_type;
    using reference = typename super_t::reference;

protected:
    iterator_range_base(Iterator begin_iterator, Iterator end_iterator):
        super_t(begin_iterator, end_iterator) {}

public:
    size_type size() const noexcept {
        return this->end_iterator - this->begin_iterator;
    }

    reference operator[](size_type i) const {
        return *(this->begin_iterator + i);
    }
};


/**
 * @class iterator_range
 * @brief Range adaptor for a pair of iterators.
 *
 * Adapts a pair of iterators into a range that can be used in range-based for loops 
 * and range algorithms.
 *
 * @tparam Iterator The type of the iterator.
 */
template <typename Iterator>
class iterator_range: public iterator_range_base<Iterator, 
    std::is_convertible_v<iterator_category_t<Iterator>, std::random_access_iterator_tag>>
{
private:
    using super_t = iterator_range_base<Iterator, 
        std::is_convertible_v<iterator_category_t<Iterator>, 
        std::random_access_iterator_tag>>;
public:
    iterator_range(Iterator begin_iterator, Iterator end_iterator):
        super_t(begin_iterator, end_iterator) {}
      
};


/**
 * @brief Helper function to create an iterator range.
 *
 * @tparam Iterator The type of the iterator.
 * @param x The start of the range.
 * @param y The end of the range.
 * @return An \c iterator_range that spans [ \p x , \p y ).
 */
template <typename Iterator> 
inline iterator_range<Iterator> make_range(Iterator x, Iterator y) {
  return iterator_range<Iterator>(std::move(x), std::move(y));
}


/**
 * @brief Helper function to create an iterator range from a pair of iterators.
 *
 * @tparam Iterator The type of the iterator.
 * @param p A pair of iterators representing the start and end of the range.
 * @return An \c iterator_range that spans [ \p p.first , \p p.second ).
 */
template <typename Iterator> 
inline iterator_range<Iterator> make_range(std::pair<Iterator, Iterator> p) {
  return iterator_range<Iterator>(std::move(p.first), std::move(p.second));
}


#endif /* PCC_ITERATOR_ITERATOR_RANGE_H */
