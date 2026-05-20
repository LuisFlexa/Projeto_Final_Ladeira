#ifndef BASIC_TYPES_HPP
#define BASIC_TYPES_HPP

#include <stdint.h>

// Define basic types for various unsigned integer sizes
typedef uint8_t u1; ///< 8-bit unsigned integer
typedef uint16_t u2; ///< 16-bit unsigned integer
typedef uint32_t u4; ///< 32-bit unsigned integer

// Enum to represent object types in a class file or virtual machine
enum ObjectType {
    CLASS_INSTANCE, ///< Represents a class instance
    STRING_INSTANCE, ///< Represents a string instance
    ARRAY ///< Represents an array instance
};
typedef enum ObjectType ObjectType; ///< Type alias for ObjectType

// Enum to represent value types used in the JVM, such as primitive types and references
enum ValueType {
    BOOLEAN, ///< Represents a boolean value
    BYTE, ///< Represents a byte value
    CHAR, ///< Represents a character value
    SHORT, ///< Represents a short integer value
    INT, ///< Represents an integer value
    FLOAT, ///< Represents a float value
    LONG, ///< Represents a long integer value
    DOUBLE, ///< Represents a double-precision floating point value
    RETURN_ADDR, ///< Represents a return address in a stack frame
    REFERENCE, ///< Represents a reference to an object (used in JVM)
    PADDING ///< Padding used to align structures
};
typedef enum ValueType ValueType; ///< Type alias for ValueType

// Forward declarations for structures and classes used in the class file format
class Object;
struct Value;
struct ConstantPoolInfo;
struct ConstClassInfo;
struct ConstFieldRefInfo;
struct ConstMethodRefInfo;
struct ConstInterfaceMethodRefInfo;
struct ConstStrInfo;
struct ConstIntInfo;
struct ConstFloatInfo;
struct ConstLongInfo;
struct ConstDoubleInfo;
struct ConstNameTypeInfo;
struct ConstUtf8Info;
struct FieldInfo;
struct AttributeInfo;
struct ConstValueAttribute;
struct ExceptionTable;
struct CodeAttribute;
struct ExceptionsAttribute;
struct Class;
struct InnerClassesAttribute;
struct SyntheticAttribute;
struct SourceFileAttribute;
struct LineNumberTable;
struct LineNumberTableAttribute;
struct LocalVariableTable;
struct LocalVariableTableAttribute;
struct DeprecatedAttribute;
struct MethodInfo;

// Structure representing a value used in the JVM (primitive or reference types)
struct Value {
    ValueType print_type; ///< Type of value to be printed (e.g., formatted output)
    ValueType type; ///< Actual value type (e.g., integer, float, reference)
    union {
        bool boolean_value; ///< Boolean value
        int8_t byte_value; ///< Byte value
        uint8_t char_value; ///< Character value (in UTF-8 encoding)
        int16_t short_value; ///< Short integer value
        int32_t int_value; ///< Integer value
        float float_value; ///< Float value
        int64_t long_value; ///< Long integer value
        double double_value; ///< Double precision floating point value
        u4 return_address; ///< Return address (for method calls)
        Object* object; ///< Object reference (pointer to a Java object)
    } data; ///< Union holding the actual data of the value
};
typedef struct Value Value; ///< Type alias for Value

// Constant pool structure types (representing various constant pool entries in a class file)
typedef struct ConstantPoolInfo ConstantPoolInfo; ///< Type alias for ConstantPoolInfo
typedef struct ConstClassInfo ConstClassInfo; ///< Type alias for ConstClassInfo
typedef struct ConstFieldRefInfo ConstFieldRefInfo; ///< Type alias for ConstFieldRefInfo
typedef struct ConstMethodRefInfo ConstMethodRefInfo; ///< Type alias for ConstMethodRefInfo
typedef struct ConstInterfaceMethodRefInfo ConstInterfaceMethodRefInfo; ///< Type alias for ConstInterfaceMethodRefInfo
typedef struct ConstStrInfo ConstStrInfo; ///< Type alias for ConstStrInfo
typedef struct ConstIntInfo ConstIntInfo; ///< Type alias for ConstIntInfo
typedef struct ConstFloatInfo ConstFloatInfo; ///< Type alias for ConstFloatInfo
typedef struct ConstLongInfo ConstLongInfo; ///< Type alias for ConstLongInfo
typedef struct ConstDoubleInfo ConstDoubleInfo; ///< Type alias for ConstDoubleInfo
typedef struct ConstNameTypeInfo ConstNameTypeInfo; ///< Type alias for ConstNameTypeInfo
typedef struct ConstUtf8Info ConstUtf8Info; ///< Type alias for ConstUtf8Info
typedef struct FieldInfo FieldInfo; ///< Type alias for FieldInfo
typedef struct AttributeInfo AttributeInfo; ///< Type alias for AttributeInfo
typedef struct ConstValueAttribute ConstValueAttribute; ///< Type alias for ConstValueAttribute
typedef struct ExceptionTable ExceptionTable; ///< Type alias for ExceptionTable
typedef struct CodeAttribute CodeAttribute; ///< Type alias for CodeAttribute
typedef struct ExceptionsAttribute ExceptionsAttribute; ///< Type alias for ExceptionsAttribute
typedef struct Class Class; ///< Type alias for Class
typedef struct InnerClassesAttribute InnerClassesAttribute; ///< Type alias for InnerClassesAttribute
typedef struct SyntheticAttribute SyntheticAttribute; ///< Type alias for SyntheticAttribute
typedef struct SourceFileAttribute SourceFileAttribute; ///< Type alias for SourceFileAttribute
typedef struct LineNumberTable LineNumberTable; ///< Type alias for LineNumberTable
typedef struct LineNumberTableAttribute LineNumberTableAttribute; ///< Type alias for LineNumberTableAttribute
typedef struct LocalVariableTable LocalVariableTable; ///< Type alias for LocalVariableTable
typedef struct LocalVariableTableAttribute LocalVariableTableAttribute; ///< Type alias for LocalVariableTableAttribute
typedef struct DeprecatedAttribute DeprecatedAttribute; ///< Type alias for DeprecatedAttribute
typedef struct MethodInfo MethodInfo; ///< Type alias for MethodInfo

enum FIELD_FLAGS {
    FIELD_FLAG_ACC_PUBLIC = 0x1,
    FIELD_FLAG_ACC_PRIVATE = 0x2,
    FIELD_FLAG_ACC_PROTECTED = 0x4,
    FIELD_FLAG_ACC_STATIC = 0x8,
    FIELD_FLAG_ACC_FINAL = 0x10,
    FIELD_FLAG_ACC_VOLATILE = 0x40,
    FIELD_FLAG_ACC_TRANSIENT = 0x80,
    FIELD_FLAG_ACC_SYNTHETIC = 0x1000,
    FIELD_FLAG_ACC_ENUM = 0x4000,
};

// Structure representing a field in a class file (member variables of a class)
struct FieldInfo {
    u2 access_flags; ///< Access flags for the field (e.g., public, static)
    u2 name_index; ///< Index into the constant pool for the field's name
    u2 descriptor_index; ///< Index into the constant pool for the field's descriptor (type)
    u2 attributes_count; ///< Number of attributes associated with the field
    AttributeInfo* attributes; ///< Pointer to an array of field attributes
};

// Structure representing a method in a class file
struct MethodInfo {
    u2 access_flags; ///< Access flags for the method (e.g., public, static)
    u2 name_index; ///< Index into the constant pool for the method's name
    u2 descriptor_index; ///< Index into the constant pool for the method's descriptor (signature)
    u2 attributes_count; ///< Number of attributes associated with the method
    AttributeInfo* attributes; ///< Pointer to an array of method attributes
};

// Enum to define the types of constants in the constant pool
typedef enum ConstType {
    ConstClass = 7, ///< Constant representing a class
    ConstFieldRef = 9, ///< Constant representing a field reference
    ConstMethodRef = 10, ///< Constant representing a method reference
    ConstInterfaceMethodRef = 11, ///< Constant representing an interface method reference
    ConstStr = 8, ///< Constant representing a string
    ConstInt = 3, ///< Constant representing an integer
    ConstFloat = 4, ///< Constant representing a float
    ConstLong = 5, ///< Constant representing a long
    ConstDouble = 6, ///< Constant representing a double
    ConstNameType = 12, ///< Constant representing a name and type pair
    ConstUtf8 = 1, ///< Constant representing a UTF-8 string
    ConstNull = 0 ///< Null constant (used in the constant pool)
} ConstType; ///< Type alias for ConstType

// Structures representing various constant pool entries
struct ConstClassInfo {
    u2 name_index; ///< Index into the constant pool for the class name
};

struct ConstFieldRefInfo {
    u2 class_index; ///< Index into the constant pool for the class
    u2 name_and_type_index; ///< Index into the constant pool for name and type
};

struct ConstMethodRefInfo {
    u2 class_index; ///< Index into the constant pool for the class
    u2 name_and_type_index; ///< Index into the constant pool for name and type
};

struct ConstInterfaceMethodRefInfo {
    u2 class_index; ///< Index into the constant pool for the class
    u2 name_and_type_index; ///< Index into the constant pool for name and type
};

struct ConstStrInfo {
    u2 string_index; ///< Index into the constant pool for the string value
};

struct ConstIntInfo {
    u4 bytes; ///< Integer value
};

struct ConstFloatInfo {
    u4 bytes; ///< Float value (4-byte IEEE 754 format)
};

struct ConstLongInfo {
    u4 high_bytes; ///< Upper 32 bits of the long value
    u4 low_bytes; ///< Lower 32 bits of the long value
};

struct ConstDoubleInfo {
    u4 high_bytes; ///< Upper 32 bits of the double value
    u4 low_bytes; ///< Lower 32 bits of the double value
};

struct ConstNameTypeInfo {
    u2 name_index; ///< Index into the constant pool for the name
    u2 descriptor_index; ///< Index into the constant pool for the descriptor
};

struct ConstUtf8Info {
    u2 length; ///< Length of the UTF-8 string
    u1* bytes; ///< UTF-8 encoded bytes of the string
};

// Structure representing an entry in the constant pool, which can be of different types
struct ConstantPoolInfo {
    u1 tag; ///< The tag indicating the type of constant (e.g., class, method, integer)
    union {
        ConstClassInfo class_info;
        ConstFieldRefInfo field_ref_info;
        ConstMethodRefInfo method_ref_info;
        ConstInterfaceMethodRefInfo interface_method_ref_info;
        ConstStrInfo str_info;
        ConstIntInfo int_info;
        ConstFloatInfo float_info;
        ConstLongInfo long_info;
        ConstDoubleInfo double_info;
        ConstNameTypeInfo name_type_info;
        ConstUtf8Info utf8_info;
    } info; ///< Union of different types of constant pool entries
};

// Attributes related to constants
struct ConstValueAttribute {
    u2 const_value_index; ///< Index into the constant pool for the constant value
};

// Exception handling information for methods
struct ExceptionTable {
    u2 start_pc; ///< Start offset of the exception handler
    u2 end_pc; ///< End offset of the exception handler
    u2 handler_pc; ///< Offset of the handler code
    u2 catch_type; ///< Type of exception being caught (or zero for any type)
};

// Code attribute representing the bytecode of a method
struct CodeAttribute {
    u2 max_stack; ///< Maximum stack size for the method's bytecode
    u2 max_locals; ///< Maximum number of local variables for the method
    u4 code_length; ///< Length of the bytecode array
    u1* code; ///< Bytecode array
    u2 exception_table_length; ///< Number of entries in the exception table
    ExceptionTable* exception_table; ///< Exception table
    u2 attributes_count; ///< Number of attributes associated with the code
    AttributeInfo* attributes; ///< Pointer to an array of attributes related to the code
};

// Attribute representing the exceptions thrown by a method
struct ExceptionsAttribute {
    u2 number_of_exceptions; ///< Number of exceptions
    u2* exception_index_table; ///< Table of exception indices
};

// Structure for inner classes within a class file
struct Class {
    u2 inner_class_info_index; ///< Index into the constant pool for inner class info
    u2 outer_class_info_index; ///< Index into the constant pool for outer class info
    u2 inner_name_index; ///< Index into the constant pool for the inner class's name
    u2 inner_class_access_flags; ///< Access flags for the inner class
};

// Inner classes attribute containing a list of inner classes
struct InnerClassesAttribute {
    u2 number_of_classes; ///< Number of inner classes
    Class* classes; ///< Pointer to an array of inner class structures
};

// Synthetic attribute indicating synthetic (non-source) methods/fields
struct SyntheticAttribute { };

// Source file attribute representing the source file of the class
struct SourceFileAttribute {
    u2 sourcefile_index; ///< Index into the constant pool for the source file name
};

// Line number table used for debugging information
struct LineNumberTable {
    u2 start_pc; ///< Start offset of the code
    u2 line_number; ///< Corresponding line number in the source file
};

// LineNumberTable attribute used for debugging
struct LineNumberTableAttribute {
    u2 line_number_table_length; ///< Number of line number entries
    LineNumberTable* line_number_table; ///< Pointer to the line number table
};

// Local variable table used for debugging information
struct LocalVariableTable {
    u2 start_pc; ///< Start offset of the variable
    u2 length; ///< Length of the variable's scope
    u2 name_index; ///< Index into the constant pool for the variable's name
    u2 descriptor_index; ///< Index into the constant pool for the variable's type descriptor
    u2 index; ///< Index of the local variable in the method's local variable table
};

// LocalVariableTableAttribute used for debugging information
struct LocalVariableTableAttribute {
    u2 local_variable_table_length; ///< Number of local variable entries
    LocalVariableTable* local_variable_table; ///< Pointer to the local variable table
};

// Deprecated attribute indicating deprecated methods/fields
struct DeprecatedAttribute { };

// Deprecated attribute 1 (duplicate definition?)
struct DeprecatedAttribute1 { };

// General attribute structure holding various possible attribute types
struct AttributeInfo {
    u2 attribute_name_index; ///< Index into the constant pool for the attribute name
    u4 attribute_length; ///< Length of the attribute
    union {
        ConstValueAttribute const_value_info;
        CodeAttribute code_info;
        ExceptionsAttribute exceptions_info;
        InnerClassesAttribute inner_classes_info;
        SyntheticAttribute synthetic_info;
        SourceFileAttribute source_file_info;
        LineNumberTableAttribute line_number_table_info;
        LocalVariableTableAttribute local_variable_table_info;
        DeprecatedAttribute deprecated_info;
        DeprecatedAttribute1 deprecated_info1;
    } info; ///< Union holding the data for the attribute
};

#endif
