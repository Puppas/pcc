#ifndef PCC_PASSES_GVN_H
#define PCC_PASSES_GVN_H


class Function;
class Module;

void global_value_numbering(Function* fn);
void global_value_numbering(Module* module);


#endif /* PCC_PASSES_GVN_H */
