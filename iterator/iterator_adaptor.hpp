#ifndef PCC_ITERATOR_ITERATOR_ADAPTOR_H
#define PCC_ITERATOR_ITERATOR_ADAPTOR_H


#include "iterator_adaptor_base.hpp"


/**
 * @class iterator_adaptor
 * @brief The \c iterator_adaptor class template adapts \c Base type to create a new iterator. 
 *
 * The core interface functions expected by \c iterator_facade are implemented in terms of 
 * the iterator_adaptor's \c Base template parameter. A class derived from \c iterator_adaptor 
 * typically redefines some of the core interface functions to adapt the behavior of the \c Base type. 
 *
 * @tparam Derived The type that the iterator is being derived into.
 * @tparam Base The base iterator.
 * @tparam Value The value type that the iterator refers to. Default is use_default.
 * @tparam Category The category of the iterator. Default is use_default.
 * @tparam Reference The reference type that the iterator refers to. Default is use_default.
 * @tparam Difference The type used to represent the difference between two iterators. Default is use_default.
 */
template<typename Derived, typename Base, typename Value = use_default,
         typename Category = use_default, typename Reference = use_default, typename Difference = use_default>
class iterator_adaptor: public iterator_adaptor_base<
                            Derived, Base, Value,
                            Category, Reference, Difference>::type
{
    friend class iterator_core_access;

private:
    using super_t = typename iterator_adaptor_base<
                        Derived, Base, Value,
                        Category, Reference, Difference>::type;
  
public:
    using base_type = Base;
    using reference = typename super_t::reference;
    using iterator_category = typename super_t::iterator_category;
    using difference_type = typename super_t::difference_type;

    /// @brief Default constructor.
    iterator_adaptor() = default;

    /**
     * @brief Construct from base iterator.
     * 
     * @param iter The base iterator to construct from.
     */
    explicit iterator_adaptor(base_type const& iter): m_iterator(iter) {}
    
    /**
     * @brief Returns the base iterator
     *
     * @return The base iterator.
     */
    base_type const& base() const { return m_iterator; }

protected:
    using iterator_adaptor_ = iterator_adaptor;
    
    base_type& base_reference() { return m_iterator; }
    base_type const& base_reference() const { return m_iterator; }

private: 
    reference dereference() const { 
        return *m_iterator; 
    }

    template<typename OtherDerived, typename OtherIterator, typename V, typename T, typename R, typename D>
    bool equal(iterator_adaptor<OtherDerived, OtherIterator, V, T, R, D> const& x) const { 
        return m_iterator == x.base(); 
    }

    /**
     * @brief Advances the iterator by n steps. Requires random-access iterator.
     *
     * @param n The number of steps to advance.
     */
    void advance(difference_type n)
    {
        static_assert(std::is_convertible_v<iterator_category, std::random_access_iterator_tag>, 
                      "advance() requires random-access ability");
        m_iterator = m_iterator + n;
    }
    
    /// @brief Moves the iterator one step forward.
    void increment() { ++m_iterator; }

    /// @brief Moves the iterator one step backward. Requires bidirectional iterator.
    void decrement()
    {
        static_assert(std::is_convertible_v<iterator_category, std::bidirectional_iterator_tag>, 
                     "only bidirectional iterators can perform decrement()");
        --m_iterator;
    }

    /**
     * @brief Computes the distance to another iterator. Requires random-access iterator.
     *
     * @param y The iterator to compute the distance to.
     * @return The distance to the iterator \p y.
     */
    template<typename OtherDerived, typename OtherIterator, typename V, typename T, typename R, typename D>
    difference_type 
    distance_to(iterator_adaptor<OtherDerived, OtherIterator, V, T, R, D> const& y) const
    {
        static_assert(std::is_convertible_v<iterator_category, std::random_access_iterator_tag>, 
                     "distance_to() requires random-access ability");
        return y.base() - m_iterator; 
    }

private:
    base_type m_iterator;
};



#endif /* PCC_ITERATOR_ITERATOR_ADAPTOR_H */
