#ifndef CLASS_FILE_READER_HPP
#define CLASS_FILE_READER_HPP

#include "BasicTypes.hpp"
#include "ClassFile.hpp"
#include <cassert>
#include <cstdbool>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

/**
 * @class ClassFileReader
 * @brief A singleton class for reading and parsing class files (e.g., Java class files).
 *
 * This class provides methods for reading and interpreting the contents of a Java class file
 * from a file stream. It parses various sections of the class file, including the magic number,
 * version, constant pool, fields, methods, and attributes, among others. The class ensures
 * correct handling of the class file format and endianess.
 */
class ClassFileReader {
public:
    /**
     * @brief Returns the singleton instance of the ClassFileReader.
     *
     * @return ClassFileReader& The singleton instance of the class.
     */
    static ClassFileReader& get_instance()
    {
        static ClassFileReader instance;
        return instance;
    }

    /**
     * @brief Destructor for ClassFileReader.
     */
    ~ClassFileReader();

    /**
     * @brief Reads a class file from the provided file stream.
     *
     * This function parses the contents of a class file from the given `FILE*` stream and
     * returns a pointer to a `ClassFile` object representing the parsed class file.
     *
     * @param file The file stream to read from.
     * @return ClassFile* A pointer to the parsed ClassFile object.
     */
    ClassFile* read_class_file(FILE* file);

private:
    /**
     * @brief Private constructor for singleton pattern.
     */
    ClassFileReader();

    /**
     * @brief Deleted copy constructor for singleton pattern.
     */
    ClassFileReader(ClassFileReader const&);

    /**
     * @brief Flag to check if the system is little endian.
     */
    bool is_little_endian;

    /**
     * @brief Determines the system's endian type.
     *
     * @return bool True if the system is little-endian, false otherwise.
     */
    bool check_endian_type();

    /**
     * @brief Reads a single byte (u1) from the file stream.
     *
     * @param file The file stream to read from.
     * @return u1 The byte read from the stream.
     */
    u1 read_u1(FILE* file);

    /**
     * @brief Reads two bytes (u2) from the file stream.
     *
     * @param file The file stream to read from.
     * @return u2 The 2-byte value read from the stream.
     */
    u2 read_u2(FILE* file);

    /**
     * @brief Reads four bytes (u4) from the file stream.
     *
     * @param file The file stream to read from.
     * @return u4 The 4-byte value read from the stream.
     */
    u4 read_u4(FILE* file);

    /**
     * @brief Reads and sets the magic number of the class file.
     *
     * @param fp The file stream to read from.
     * @param class_file The ClassFile object to set the magic number in.
     */
    void set_magic(FILE* fp, ClassFile* class_file);

    /**
     * @brief Verifies if the magic number in the class file is valid.
     *
     * @param class_file The ClassFile object to check.
     * @return bool True if the magic number is valid, false otherwise.
     */
    bool check_magic_valid(ClassFile* class_file);

    /**
     * @brief Reads and sets the version information (major and minor) of the class file.
     *
     * @param fp The file stream to read from.
     * @param class_file The ClassFile object to set the version in.
     */
    void set_version(FILE* fp, ClassFile* class_file);

    /**
     * @brief Validates the class file version.
     *
     * @param class_file The ClassFile object to validate.
     * @param major The expected major version number.
     * @return bool True if the version is valid, false otherwise.
     */
    bool is_version_valid(ClassFile* class_file, uint16_t major);

    /**
     * @brief Reads and sets the constant pool size and its contents.
     *
     * @param fp The file stream to read from.
     * @param class_file The ClassFile object to store the constant pool.
     */
    void set_constant_pool(FILE* fp, ClassFile* class_file);
    void set_constant_pool_size(FILE* fp, ClassFile* class_file);

    /**
     * @brief Reads constant pool entries.
     * These methods read various constant pool types, including class references,
     * field and method references, string values, and more.
     */
    ConstClassInfo get_const_class_info(FILE* fp);
    ConstFieldRefInfo get_const_field_ref_info(FILE* fp);
    ConstMethodRefInfo get_const_method_ref_info(FILE* fp);
    ConstInterfaceMethodRefInfo get_const_interface_method_ref_info(FILE* fp);
    ConstStrInfo get_const_str_info(FILE* fp);
    ConstIntInfo get_const_int_info(FILE* fp);
    ConstFloatInfo get_const_float_info(FILE* fp);
    ConstLongInfo get_const_long_info(FILE* fp);
    ConstDoubleInfo get_const_double_info(FILE* fp);
    ConstNameTypeInfo get_const_name_type_info(FILE* fp);
    ConstUtf8Info get_const_utf8_info(FILE* fp);

    /**
     * @brief Reads and sets various class file components.
     * These methods set the class file's access flags, this class, super class,
     * interfaces, fields, methods, and attributes.
     */
    void set_access_flags(FILE* fp, ClassFile* class_file);
    void set_this_class(FILE* fp, ClassFile* class_file);
    void set_super_class(FILE* fp, ClassFile* class_file);
    void set_interfaces_count(FILE* fp, ClassFile* class_file);
    void set_interfaces(FILE* fp, ClassFile* class_file);
    void set_fields_count(FILE* fp, ClassFile* class_file);
    void set_fields(FILE* fp, ClassFile* class_file);

    /**
     * @brief Retrieves various types of attributes.
     * These methods parse the attributes section of the class file,
     * which contains metadata and additional information about the class.
     */
    AttributeInfo get_attribute_info(FILE* fp, ClassFile* class_file);
    ConstValueAttribute get_attribute_constant_value(FILE* fp);
    ExceptionTable get_exception_table(FILE* fp);
    CodeAttribute get_attribute_code(FILE* fp, ClassFile* class_file);
    ExceptionsAttribute get_attribute_exceptions(FILE* fp);
    Class get_class(FILE* fp);
    InnerClassesAttribute get_attribute_inner_classes(FILE* fp);
    SyntheticAttribute get_attribute_synthetic();
    SourceFileAttribute get_attribute_source_file(FILE* fp);
    LineNumberTable get_line_number_table(FILE* fp);
    LineNumberTableAttribute get_attribute_line_number_table(FILE* fp);
    LocalVariableTable get_local_variable_table(FILE* fp);
    LocalVariableTableAttribute get_attribute_local_variable(FILE* fp);
    DeprecatedAttribute get_attribute_deprecated();

    /**
     * @brief Retrieves the UTF-8 string from the constant pool by index.
     *
     * @param index The index of the constant pool entry.
     * @param class_file The ClassFile object containing the constant pool.
     * @return ConstUtf8Info The UTF-8 constant string.
     */
    ConstUtf8Info get_utf8_from_constant_pool(u2 index, ClassFile* class_file);

    /**
     * @brief Reads and sets the methods section of the class file.
     *
     * @param fp The file stream to read from.
     * @param class_file The ClassFile object to store the methods.
     */
    void set_methods(FILE* fp, ClassFile* class_file);
    void set_methods_count(FILE* fp, ClassFile* class_file);

    /**
     * @brief Reads and sets the attributes section of the class file.
     *
     * @param fp The file stream to read from.
     * @param class_file The ClassFile object to store the attributes.
     */
    void set_attributes(FILE* fp, ClassFile* class_file);
    void set_attributes_count(FILE* fp, ClassFile* class_file);

    /**
     * @brief Free memory associated with a given class file.
     *
     * @param class_file The ClassFile object to store the attributes.
     */
    void free_class_file(ClassFile* class_file);
};

#endif
