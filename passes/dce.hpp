#ifndef PCC_PASSES_DCE_H
#define PCC_PASSES_DCE_H


class Module;
class Function;


void dead_code_elimination(Function* fn);
void dead_code_elimination(Module* module);


#endif /* PCC_PASSES_DCE_H */
