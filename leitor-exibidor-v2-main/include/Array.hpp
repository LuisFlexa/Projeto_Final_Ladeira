/*
 * Array.hpp
 *
 * This header file defines the Array class, which represents an array structure in a JVM 8 runtime environment.
 * The Array class extends the Object class and provides methods for managing dynamically sized collections
 * of values of a specific ValueType.
 *
 * Dependencies:
 * - BasicTypes.hpp: Defines fundamental value types used in the JVM.
 * - Object.hpp: Base class for all objects in the JVM.
 * - <vector>: Used for storing array elements dynamically.
 * - <cassert>, <cstdlib>, <iostream>: Standard C++ utilities.
 */

#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "BasicTypes.hpp"
#include "Object.hpp"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

/**
 * @class Array
 * @brief Represents a JVM array object that can store values of a specific type.
 */
class Array : public Object {

public:
    /**
     * @brief Constructs an Array object with a specified content type.
     * @param type The type of values this array will store.
     */
    Array(ValueType type);

    /**
     * @brief Destructor to clean up resources.
     */
    ~Array();

    /**
     * @brief Retrieves the object type of this array.
     * @return The object type of the array.
     */
    ObjectType object_type();

    /**
     * @brief Gets the type of values stored in the array.
     * @return The ValueType of the array contents.
     */
    ValueType array_content_type();

    /**
     * @brief Adds a value to the end of the array.
     * @param value The value to be added.
     */
    void push_value(Value value);

    /**
     * @brief Removes a value at a specified index.
     * @param index The position of the value to remove.
     * @return The removed Value.
     */
    Value remove_at(uint32_t index);

    /**
     * @brief Removes the last value in the array.
     * @return The removed Value.
     */
    Value remove_last();

    /**
     * @brief Removes the first value in the array.
     * @return The removed Value.
     */
    Value remove_first();

    /**
     * @brief Retrieves the number of elements in the array.
     * @return The size of the array.
     */
    uint32_t get_size();

    /**
     * @brief Retrieves the value at a specific index.
     * @param index The position of the value to retrieve.
     * @return The Value at the specified index.
     */
    Value get_value(uint32_t index);

    /**
     * @brief Changes the value at a given index.
     * @param index The position to modify.
     * @param value The new Value to set.
     */
    void change_value(uint32_t index, Value value);

private:
    ValueType array_type; ///< The type of values stored in the array.
    vector<Value> array_elements; ///< The dynamic storage for array elements.
};

#endif // ARRAY_HPP
