#ifndef STR_OBJECT_HPP
#define STR_OBJECT_HPP

#include "Object.hpp"
#include <cstdlib>
#include <string>

using namespace std;

/**
 * @brief Represents a Java String object in the JVM.
 *
 * In the context of a JVM 8 implementation, the StrObject class is used to model
 * Java's immutable String objects. It extends the base Object class (which represents
 * the common structure of all objects in the JVM) and provides methods to access and
 * modify the underlying string value.
 */
class StrObject : public Object {

public:
    /**
     * @brief Constructs a StrObject with an optional initial string.
     *
     * This constructor initializes a new string object with the provided value.
     * If no value is provided, the internal string defaults to an empty string.
     *
     * @param s The initial string value for the object. Defaults to an empty string.
     */
    StrObject(string s = "");

    /**
     * @brief Destructor for StrObject.
     *
     * Cleans up any resources allocated by the StrObject. For this implementation,
     * standard string cleanup will be handled automatically.
     */
    ~StrObject();

    /**
     * @brief Retrieves the type of the object.
     *
     * This method returns the ObjectType that indicates this object is a string.
     * This is useful for type-checking and casting operations within the JVM.
     *
     * @return The ObjectType representing a string object.
     */
    ObjectType object_type();

    /**
     * @brief Gets the current string value.
     *
     * Returns the internal string value stored in the object. This allows the JVM
     * or other components to retrieve the contents of the Java string.
     *
     * @return A std::string representing the current value of the string object.
     */
    string get_str();

    /**
     * @brief Sets or updates the string value.
     *
     * Updates the internal string to the provided value. Although Java strings are
     * immutable, this method can be used internally by the JVM implementation during
     * initialization or when constructing new string objects.
     *
     * @param s The new string value to store in the object.
     */
    void set_str(string s);

private:
    /**
     * @brief The internal string storage.
     *
     * Holds the actual sequence of characters for the Java string. This member
     * variable encapsulates the data that the StrObject represents.
     */
    string internal_str;
};

#endif
