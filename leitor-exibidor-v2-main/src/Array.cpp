#include "Array.hpp"

Array::Array(ValueType type)
    : array_type{type}
{

}

Array::~Array()
{

}

ObjectType Array::object_type()
{
    return ARRAY;
}

ValueType Array::array_content_type()
{
    return array_type;
}

void Array::push_value(Value value)
{
    array_elements.push_back(value);
}

Value Array::remove_at(uint32_t index)
{
    if (index > array_elements.size() || index < 0) {
        Value invalid_return = Value{ INT, INT, {.int_value = -1} };
        cerr << "Array::remove_at - Invalid index value";
        return invalid_return;
    };
    Value temp = array_elements[index];
    array_elements.erase(array_elements.begin() + index);
    return temp;
}

Value Array::remove_last()
{
    Value temp = array_elements.back();
    array_elements.pop_back();
    return temp;
}

Value Array::remove_first()
{
    Value temp = array_elements.front();
    array_elements.pop_back();
    return temp;
}

uint32_t Array::get_size()
{
    return array_elements.size();
}

Value Array::get_value(uint32_t index)
{
    if (index > array_elements.size() || index < 0) {
        Value invalid_return = Value{ INT, INT, {.int_value = -1} };
        cerr << "Array::get_value - Invalid index value";
        return invalid_return;
    }
    return array_elements[index];
}

void Array::change_value(uint32_t index, Value value)
{
    if (index > array_elements.size() || index < 0) {
        cerr << "Array::change_value- Invalid index value";
        return;
    }
    array_elements[index] = value;
}
