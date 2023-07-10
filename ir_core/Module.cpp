#include "Module.hpp"
#include "IRContext.hpp"
#include "IRPrinter.hpp"


Module::Module(IRContext& context): 
    context(context), gvars(&sym_tab), functions(&sym_tab) 
{
    context.add_module(this);
}

Module::~Module()
{
    context.remove_module(this);
    for (auto &&function : functions)
        function.drop_all_references();
}

GlobalVariable* Module::get_or_insert_global(Type* ty, const std::string& name) {
    if (sym_tab.find(name) != sym_tab.end()) {
        return cast<GlobalVariable>(sym_tab[name]);
    }

    return new GlobalVariable(ty, name, this);
}

Function* Module::get_or_insert_funtion(Type* ty, const std::string& name) {
    if (sym_tab.find(name) != sym_tab.end()) {
        return cast<Function>(sym_tab[name]);
    }

    return Function::create(ty, name, this);
}

GlobalVariable *Module::get_global(const std::string& name) {
    return cast<GlobalVariable>(sym_tab[name]);
}

Function *Module::get_function(const std::string& name) {
    return cast<Function>(sym_tab[name]);
}


std::ostream& operator<<(std::ostream& os, const Module& module)
{
    IRPrinter printer;
    printer.print(&module, os);
    return os;
}