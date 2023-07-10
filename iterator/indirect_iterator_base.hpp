#ifndef PCC_ITERATOR_INDIRECT_ITERATOR_BASE_H
#define PCC_ITERATOR_INDIRECT_ITERATOR_BASE_H

#include "iterator_adaptor.hpp"
#include "utils/type_traits.hpp"


template<typename Derived, typename Base, typename Value,
         typename Category, typename Reference, typename Difference>
struct indirect_iterator_base
{
    using Value_ = eval_if_t<std::is_same_v<Value, use_default>, 
                        iterator_value<iterator_value_t<Base>>,
                        identity<Value>>;

    using Reference_ = eval_if_t<std::is_same_v<Reference, use_default>, 
                            eval_if<std::is_same_v<Value, use_default>, 
                                iterator_reference<iterator_value_t<Base>>, 
                                identity<Value&>>, 
                            identity<Reference>>;

    using type = iterator_adaptor<Derived, Base, Value_, Category, Reference_, Difference>;
}; 



#endif /* PCC_ITERATOR_INDIRECT_ITERATOR_BASE_H */
