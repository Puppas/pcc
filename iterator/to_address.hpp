#ifndef PCC_ITERATOR_TO_ADDRESS_H
#define PCC_ITERATOR_TO_ADDRESS_H


#include "utils/type_traits.hpp"


template<typename T>
using has_pointer_traits_to_address_op = decltype(std::pointer_traits<T>::to_address(std::declval<T>()));

template<typename T>
using has_pointer_traits_to_address = is_detected<has_pointer_traits_to_address_op, T>;

template<typename T>
constexpr bool has_pointer_traits_to_address_v = has_pointer_traits_to_address<T>::value;


template<typename T>
inline T* to_address(T* p) noexcept
{
    static_assert(!std::is_function_v<T>, "the argument can't be a function");
    return p;
}

template<typename T>
inline auto to_address(const T& p) noexcept
{
    if constexpr (has_pointer_traits_to_address_v<T>)
        return std::pointer_traits<T>::to_address(p);
    else
        return to_address(p.operator->());
}



#endif /* PCC_ITERATOR_TO_ADDRESS_H */
