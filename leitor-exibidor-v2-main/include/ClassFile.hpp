#ifndef CLASS_FILE_HPP
#define CLASS_FILE_HPP

#include "BasicTypes.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

/**
 * @class ClassFile
 * @brief Represents a Java class file.
 *
 * The `ClassFile` class encapsulates the structure of a Java class file as defined by the Java Virtual Machine (JVM) specification.
 * It contains the necessary fields for representing the magic number, version information, constant pool, fields, methods, and attributes.
 * This class is used for reading, parsing, and manipulating class files within a JVM implementation or a class file viewer tool.
 */
class ClassFile {
public:
    /**
     * @brief Default constructor for the ClassFile class.
     *
     * Initializes all the member variables to default values.
     */
    ClassFile();

    /**
     * @var magic
     * @brief The magic number of the class file (0xCAFEBABE).
     *
     * The magic number is a constant value at the beginning of the class file, used to identify valid class files.
     */
    u4 magic;

    /**
     * @var minor_version
     * @brief The minor version of the class file format.
     *
     * Represents the minor version of the Java class file format. Together with the major version, it helps determine
     * compatibility between the class file and the JVM.
     */
    u2 minor_version;

    /**
     * @var major_version
     * @brief The major version of the class file format.
     *
     * Represents the major version of the Java class file format. It is used along with the minor version to
     * verify the compatibility of the class file with a particular JVM version.
     */
    u2 major_version;

    /**
     * @var constant_pool_count
     * @brief The number of entries in the constant pool.
     *
     * The constant pool is a table of constants (e.g., strings, class references, method references) used by the class.
     */
    u2 constant_pool_count;

    /**
     * @var constant_pool
     * @brief A pointer to the array of `ConstantPoolInfo` objects representing the constant pool.
     *
     * The constant pool holds various constant values referenced by the class (e.g., strings, class types, field/method references).
     */
    ConstantPoolInfo* constant_pool;

    /**
     * @var access_flags
     * @brief The access flags of the class (e.g., public, final, abstract).
     *
     * These flags describe the visibility and other characteristics of the class (e.g., whether it's public, final, etc.).
     */
    u2 access_flags;

    /**
     * @var this_class
     * @brief The index into the constant pool that points to the class's name.
     *
     * This field stores a reference to the constant pool entry that represents the name of the class.
     */
    u2 this_class;

    /**
     * @var super_class
     * @brief The index into the constant pool that points to the superclass's name.
     *
     * Similar to `this_class`, this field references the class's superclass in the constant pool.
     * If the class has no superclass (i.e., it's the root class `java.lang.Object`), this field is zero.
     */
    u2 super_class;

    /**
     * @var interfaces_count
     * @brief The number of interfaces implemented by the class.
     *
     * This field indicates how many interfaces are implemented by the class. The list of interfaces follows this field.
     */
    u2 interfaces_count;

    /**
     * @var interfaces
     * @brief A pointer to an array of interface indices.
     *
     * The `interfaces` array holds the indices into the constant pool for the interfaces implemented by the class.
     */
    u2* interfaces;

    /**
     * @var fields_count
     * @brief The number of fields declared by the class.
     *
     * This field represents how many fields (variables) are declared in the class. The `fields` array stores the
     * details of each field.
     */
    u2 fields_count;

    /**
     * @var fields
     * @brief A pointer to an array of `FieldInfo` objects.
     *
     * The `fields` array holds detailed information about each field declared in the class, such as its name,
     * type, and access modifiers.
     */
    FieldInfo* fields;

    /**
     * @var methods_count
     * @brief The number of methods declared by the class.
     *
     * This field indicates how many methods are declared in the class. The `methods` array stores the
     * details of each method.
     */
    u2 methods_count;

    /**
     * @var methods
     * @brief A pointer to an array of `MethodInfo` objects.
     *
     * The `methods` array holds detailed information about each method declared in the class, including method
     * names, parameter types, return types, and attributes.
     */
    MethodInfo* methods;

    /**
     * @var attributes_count
     * @brief The number of attributes present in the class.
     *
     * This field indicates how many attributes are associated with the class file, such as source file information,
     * inner class information, code, exceptions, etc.
     */
    u2 attributes_count;

    /**
     * @var attributes
     * @brief A pointer to an array of `AttributeInfo` objects.
     *
     * The `attributes` array holds information about the various attributes associated with the class,
     * such as code, exceptions, inner classes, and synthetic attributes.
     */
    AttributeInfo* attributes;
};

#endif
