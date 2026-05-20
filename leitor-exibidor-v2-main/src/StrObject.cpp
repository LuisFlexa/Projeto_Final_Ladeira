#include "StrObject.hpp"

StrObject::StrObject(string s) :
    internal_str(s)
{
}

StrObject::~StrObject()
{
}

ObjectType StrObject::object_type()
{
  return ObjectType::STRING_INSTANCE;
}

string StrObject::get_str()
{
  return internal_str;
}

void StrObject::set_str(string s)
{
  internal_str = s;
}
