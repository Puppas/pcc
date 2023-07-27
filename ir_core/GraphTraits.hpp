#ifndef PCC_IR_CORE_GRAPHTRAITS_H
#define PCC_IR_CORE_GRAPHTRAITS_H



/**
 * @brief A primary template for graph traits of a graph type.
 * 
 * This primary template serves as a basis for specialization 
 * based on specific graph types. It provides a unified interface to 
 * access a graph.
 */
template<class GraphType>
struct GraphTraits {};


/**
 * @brief A primary template for inverse graph traits of a graph type.
 * 
 * Similar to \c GraphTraits, but intended for cases where the direction of edges 
 * in the graph needs to be reversed.
 */
template <class GraphType>
struct InverseGraphTraits {};





#endif /* PCC_IR_CORE_GRAPHTRAITS_H */
