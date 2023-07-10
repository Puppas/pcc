#include "GlobalObject.hpp"
#include "IRContext.hpp"


IRContext& GlobalObject::get_context() const
{
    return get_parent()->get_context();
}


std::string GlobalObject::get_name() const
{
    return get_context().get_name(this);
}


void GlobalObject::set_name(const std::string& name)
{
    return get_context().set_name(this, name);
}


GlobalObject::~GlobalObject() {
    get_context().delete_name(this);
}


