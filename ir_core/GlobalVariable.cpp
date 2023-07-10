#include "Module.hpp"


GlobalVariable::GlobalVariable(Type* ty, const std::string& name, Module* parent):
    GlobalObject(ty, ValueKind::GLOBAL_VARIABLE, name, parent)  
{
    parent->get_global_list().push_back(this);
}


symbol_table_list<GlobalVariable>::iterator GlobalVariable::erase_from_parent()
{
    get_parent()->get_global_list().erase(this);
}