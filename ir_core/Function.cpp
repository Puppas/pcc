#include "Module.hpp"


Function::Function(Type* ty, const std::string& name, Module* parent): 
    GlobalObject(ty, ValueKind::FUNCTION, name, parent) 
{
    build_params();
    parent->get_function_list().push_back(this);
}


void Function::build_params()
{
    Type* func_ty = get_value_type();
    for (Type* ty = func_ty->params; ty; ty = ty->next)
        params.push_back(new Value(ty, ValueKind::VALUE));
}

Function::~Function()
{
    drop_all_references();
    for (auto iter = params.begin(); iter != params.end(); ++iter) {
        delete *iter;
    }
}

void Function::drop_all_references()
{
    for (auto &&bb : get_bb_list())
        bb.drop_all_references();
}


symbol_table_list<Function>::iterator Function::erase_from_parent()
{
    return get_parent()->get_function_list().erase(this);
}