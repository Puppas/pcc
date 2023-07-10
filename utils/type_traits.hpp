#ifndef PCC_UTILS_TYPE_TRAITS_H
#define PCC_UTILS_TYPE_TRAITS_H


#include <type_traits>
#include <iterator>


template<typename T>
struct identity
{
    using type = T;
}; // end identity


template <bool, typename Then, typename Else>
struct eval_if
{
}; // end eval_if

template<typename Then, typename Else>
struct eval_if<true, Then, Else>
{
    using type = typename Then::type;
}; // end eval_if

template<typename Then, typename Else>
struct eval_if<false, Then, Else>
{
    using type = typename Else::type;
}; // end eval_if


template <bool Condition, typename Then, typename Else>
using eval_if_t = typename eval_if<Condition, Then, Else>::type;


template<typename Iterator>
struct iterator_value
{
    using type = typename std::iterator_traits<Iterator>::value_type;
}; // end iterator_value

template <typename Iterator>
using iterator_value_t = typename iterator_value<Iterator>::type;


template<typename Iterator>
struct iterator_category
{
    using type = typename std::iterator_traits<Iterator>::iterator_category;
}; // end iterator_category

template <typename Iterator>
using iterator_category_t = typename iterator_category<Iterator>::type;


template<typename Iterator>
struct iterator_pointer
{
    using type = typename std::iterator_traits<Iterator>::pointer;
};

template<typename Iterator>
using iterator_pointer_t = typename iterator_pointer<Iterator>::type;


template<typename Iterator>
struct iterator_reference
{
    using type =  typename std::iterator_traits<Iterator>::reference;
}; // end iterator_reference

template <typename Iterator>
using iterator_reference_t = typename iterator_reference<Iterator>::type;


template<typename Iterator>
struct iterator_difference
{
    using type =  typename std::iterator_traits<Iterator>::difference_type;
}; // end iterator_difference


template <typename Iterator>
using iterator_difference_t = typename iterator_difference<Iterator>::type;


template<typename T1, typename T2, typename T = void>
struct enable_if_convertible: std::enable_if< std::is_convertible_v<T1,T2>, T >
{};

template<typename T1, typename T2, typename T = void>
using enable_if_convertible_t = typename enable_if_convertible<T1, T2, T>::type;


template<typename T1, typename T2, typename Result = void>
struct enable_if_interoperable: 
    std::enable_if<std::is_convertible_v<T1, T2> || std::is_convertible_v<T2, T1>, Result>
{};

template<typename T1, typename T2, typename Result = void>
using enable_if_interoperable_t = typename enable_if_interoperable<T1, T2, Result>::type;


template<typename Iterator1, typename Iterator2>
struct decide_facade_diff
{
    using type = std::conditional_t<std::is_convertible_v<Iterator2, Iterator1>, 
                                    typename Iterator1::difference_type, 
                                    typename Iterator2::difference_type>;
};

template<typename Iterator1, typename Iterator2>
using decide_facade_diff_t = typename decide_facade_diff<Iterator1, Iterator2>::type;



template <typename T>
struct is_reference_to_const : std::false_type
{};

template <typename T>
struct is_reference_to_const<T const&> : std::true_type
{};

template<typename T>
inline constexpr bool is_reference_to_const_v = is_reference_to_const<T>::value;


template <typename Value, typename Reference>
struct is_iterator_writable: std::conditional_t<!(std::is_const_v<Reference> || std::is_const_v<Value> || 
        is_reference_to_const_v<Reference>), std::true_type, std::false_type>
{};

template <typename Value, typename Reference>
inline constexpr bool is_iterator_writable_v = is_iterator_writable<Value, Reference>::value;


template <typename, template <typename...> class Op, typename... T>
struct detector : std::false_type {};

template <template <typename...> class Op, typename... T>
struct detector<std::void_t<Op<T...>>, Op, T...> : std::true_type {};    

template <template <typename...> class Op, typename... T>
using is_detected = detector<void, Op, T...>;



#endif /* PCC_UTILS_TYPE_TRAITS_H */
