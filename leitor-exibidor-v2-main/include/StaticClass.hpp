#ifndef STATIC_CLASS_HPP
#define STATIC_CLASS_HPP

#include "ClassFile.hpp"
#include "ClassFileReader.hpp"
#include <map>
#include <string>

using namespace std;

/**
 * @brief Represents the static portion of a Java class.
 *
 * In the JVM 8 context, a Java class is not only defined by its instance fields and methods,
 * but also by its static fields and static initializers. The StaticClass module encapsulates the
 * class's metadata (stored in a ClassFile) as well as a collection of its static fields. This allows
 * the JVM to access and modify static variables during execution.
 */
class StaticClass {

public:
    /**
     * @brief Constructor that associates a ClassFile with its static fields.
     *
     * This constructor initializes a new StaticClass instance with a pointer to the ClassFile
     * representing the class. The ClassFile contains metadata such as the class name, version,
     * constant pool, and definitions of its fields and methods.
     *
     * @param class_file Pointer to a ClassFile object containing the class metadata.
     */
    StaticClass(ClassFile* class_file);

    /**
     * @brief Retrieves the associated ClassFile.
     *
     * Returns the pointer to the ClassFile provided at construction, which contains
     * the metadata of the Java class.
     *
     * @return Pointer to the ClassFile representing the class metadata.
     */
    ClassFile* get_class_file();

    /**
     * @brief Inserts or updates a static field's value.
     *
     * In a Java class, static fields are shared across all instances and are managed
     * separately from instance-specific data. This method allows the insertion or updating
     * of a static field's value. The static fields are stored in an internal map, where the
     * key is the field name and the value is its corresponding data.
     *
     * @param value The value to be stored in the static field.
     * @param field_name The name of the static field.
     */
    void insert_value_into_field(Value value, const string& field_name);

    /**
     * @brief Retrieves the value of a static field.
     *
     * Accesses the static field value associated with the given field name.
     * If the field does not exist, the behavior is determined by the implementation;
     * typically, an error might be raised or a default value returned.
     *
     * @param field_name The name of the static field.
     * @return The value associated with the specified static field.
     */
    Value get_value_from_field(string field_name);

    /**
     * @brief Checks whether a static field exists.
     *
     * Determines if a static field with the given name is present in the class.
     *
     * @param field_name The name of the static field to check.
     * @return True if the static field exists; false otherwise.
     */
    bool field_exists(const string& field_name);

    /**
     * @brief Performs initialization of the class file.
     *
     * This method is responsible for any static initialization required for the class.
     * It may process command-line arguments (or other initialization parameters) to set up
     * the static fields or execute static initializers as specified by the class definition.
     *
     * @param argv Command-line arguments passed to the JVM.
     * @return A string indicating the status or result of the initialization.
     */
    string initialize_file(char* argv[]);

private:
    /**
     * @brief The ClassFile associated with this static class.
     *
     * The ClassFile object holds metadata for the Java class, including its name, version,
     * constant pool, and details about its fields and methods. This metadata is used by the JVM
     * during class loading and verification.
     */
    ClassFile* class_file;

    /**
     * @brief Mapping of static field names to their corresponding values.
     *
     * This map stores the values of the class's static fields, allowing for fast lookup,
     * insertion, and modification of static data.
     */
    map<string, Value> static_fields;
};

#endif
