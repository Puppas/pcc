#ifndef PCC_GEN_IR_H
#define PCC_GEN_IR_H


class Module;
class IRContext;
struct Obj;

Module* gen_ir(Obj *prog, IRContext& context);


#endif /* PCC_GEN_IR_H */
