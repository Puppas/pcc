#ifndef PCC_ITERATOR_ITERATOR_ADAPTOR_BASE_H
#define PCC_ITERATOR_ITERATOR_ADAPTOR_BASE_H


#include "iterator_facade.hpp"
#include "utils/use_default.hpp"


// A metafunction which computes an iterator_adaptor's base class,
// a specialization of iterator_facade.
template<typename Derived, typename Base, typename Value,
         typename Category, typename Reference, typename Difference>
struct iterator_adaptor_base
{
    
    using Value_ = eval_if_t<std::is_same_v<Value, use_default>, 
                        iterator_value<Base>,
                        identity<Value>>;

    using Category_ = eval_if_t<std::is_same_v<Category, use_default>, 
                        iterator_category<Base>, 
                        identity<Category>>;

    using Reference_ = eval_if_t<std::is_same_v<Reference, use_default>, 
                            eval_if<std::is_same_v<Value, use_default>, 
                                iterator_reference<Base>, 
                                identity<Value&>>, 
                            identity<Reference>>;

    using Difference_ = eval_if_t<std::is_same_v<Difference, use_default>,
                            iterator_difference<Base>, 
                            identity<Difference>>;


    using type = iterator_facade<Derived, Value_, Category_, Reference_, Difference_>;
    
}; // end iterator_adaptor_base



#endif /* PCC_ITERATOR_ITERATOR_ADAPTOR_BASE_H */
