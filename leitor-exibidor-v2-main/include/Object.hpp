#ifndef OBJECT_HPP
#define OBJECT_HPP

/**
 * @file Object.hpp
 * @brief Defines the base Object class in a JVM 8 environment.
 *
 * The Object class serves as the root of the object hierarchy in the JVM runtime.
 * It defines a pure virtual method for retrieving the object's type, ensuring that
 * all derived classes implement this functionality.
 */

#include "BasicTypes.hpp"

/**
 * @class Object
 * @brief Abstract base class representing a JVM object.
 *
 * The Object class provides a common interface for all objects in the JVM runtime.
 * It enforces the implementation of the object_type() method to return the specific
 * type of the object.
 */
class Object {
public:
    /**
     * @brief Retrieves the type of the object.
     * @return The ObjectType representing the type of the object.
     *
     * This is a pure virtual method, meaning all subclasses must implement it.
     */
    virtual ObjectType object_type() = 0;
};

#endif // OBJECT_HPP
