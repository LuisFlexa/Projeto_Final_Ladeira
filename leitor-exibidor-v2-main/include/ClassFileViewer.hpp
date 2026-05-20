#ifndef CLASS_FILE_VIEWER_HPP
#define CLASS_FILE_VIEWER_HPP

#include "BasicTypes.hpp"
#include "ClassFile.hpp"
#include "ClassFileReader.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

/**
 * @brief Writes the content of a ClassFile object to an output file stream.
 *
 * This function writes the entire class file, including its general information,
 * constant pool, interfaces, fields, methods, and attributes, to the specified output file.
 *
 * @param class_file A pointer to the ClassFile object to be written.
 * @param output A pointer to the file stream to write to.
 */
void write_class_file(ClassFile* class_file, FILE* output);

/**
 * @brief Writes general information about the class to the standard output.
 *
 * This function displays general details about the class file, including the magic number, version,
 * and other metadata, but not the specific contents like constant pool, methods, etc.
 *
 * @param class_file A pointer to the ClassFile object whose general info is to be written.
 */
void write_general_info(ClassFile* class_file);

/**
 * @brief Writes the constant pool information of the class to the standard output.
 *
 * This function writes the details of the constant pool (a table of constants such as strings, class references,
 * method references, etc.) to the output.
 *
 * @param class_file A pointer to the ClassFile object whose constant pool is to be written.
 */
void write_constant_pool(ClassFile* class_file);

/**
 * @brief Writes the list of interfaces implemented by the class to the standard output.
 *
 * This function writes the interface information for the class, including the names and references
 * of the interfaces that the class implements.
 *
 * @param class_file A pointer to the ClassFile object whose interface information is to be written.
 */
void write_interfaces(ClassFile* class_file);

/**
 * @brief Writes the fields declared by the class to the standard output.
 *
 * This function writes information about the fields (e.g., attributes and types) declared in the class.
 *
 * @param class_file A pointer to the ClassFile object whose fields are to be written.
 */
void write_fields(ClassFile* class_file);

/**
 * @brief Writes the methods declared by the class to the standard output.
 *
 * This function writes information about the methods (e.g., names, signatures, attributes) defined in the class.
 *
 * @param class_file A pointer to the ClassFile object whose methods are to be written.
 */
void write_methods(ClassFile* class_file);

/**
 * @brief Writes the attributes of the class to the standard output.
 *
 * This function writes the class's attribute information, such as code, exceptions, source file, etc.
 *
 * @param class_file A pointer to the ClassFile object whose attributes are to be written.
 */
void write_attributes(ClassFile* class_file);

/**
 * @brief Writes detailed information about a specific attribute in the class.
 *
 * This function writes detailed information about a single attribute, including its name, type, and value.
 * It may also write the constant pool references for that attribute.
 *
 * @param attribute_info The attribute's information to be written.
 * @param index The index of the attribute within the class file.
 * @param constant_pool A pointer to the constant pool, used to format constant references.
 * @param indentation The number of spaces to indent for formatting purposes.
 */
void write_attribute_info(AttributeInfo attribute_info, uint32_t index, ConstantPoolInfo* constant_pool, uint8_t indentation);

/**
 * @brief Writes the bytecode instructions of a method to the output.
 *
 * This function writes the bytecode instructions of a method (stored in a CodeAttribute) to the output,
 * along with constant pool references and proper indentation for readability.
 *
 * @param code_attribute The CodeAttribute object containing the bytecode instructions.
 * @param constant_pool A pointer to the constant pool, used for resolving constant references.
 * @param indentation The number of spaces to indent for formatting purposes.
 */
void write_file_bytecode(CodeAttribute code_attribute, ConstantPoolInfo* constant_pool, uint8_t indentation);

/**
 * @brief Retrieves the access flags of a class, field, or method in a human-readable format.
 *
 * This function returns a formatted string representing the access flags (e.g., public, private, static, etc.)
 * corresponding to the given access flag value.
 *
 * @param access_flags The access flag value (a 2-byte unsigned integer).
 * @return const char* A string representation of the access flags.
 */
const char* get_access_flags(u2 access_flags);

/**
 * @brief Retrieves the formatted constant from the constant pool based on the given index.
 *
 * This function resolves a constant pool entry by its index and returns the constant in a human-readable format.
 *
 * @param constant_pool A pointer to the constant pool to resolve the constant from.
 * @param index The index of the constant pool entry to be formatted.
 * @return const char* The formatted constant value.
 */
const char* get_formatted_constant(ConstantPoolInfo* constant_pool, u2 index);

/**
 * @brief A static array of function names corresponding to JVM bytecodes.
 *
 * This array contains a list of all valid JVM bytecode instruction names, indexed by their opcode value.
 * It can be used for resolving opcodes to their respective names during bytecode inspection or disassembly.
 */
static const string function_names_array[] = {
    "nop",
    "aconst_null",
    "iconst_m1",
    "iconst_0",
    "iconst_1",
    "iconst_2",
    "iconst_3",
    "iconst_4",
    "iconst_5",
    "lconst_0",
    "lconst_1",
    "fconst_0",
    "fconst_1",
    "fconst_2",
    "dconst_0",
    "dconst_1",
    "bipush",
    "sipush",
    "ldc",
    "ldc_w",
    "ldc2_w",
    "iload",
    "lload",
    "fload",
    "dload",
    "aload",
    "iload_0",
    "iload_1",
    "iload_2",
    "iload_3",
    "lload_0",
    "lload_1",
    "lload_2",
    "lload_3",
    "fload_0",
    "fload_1",
    "fload_2",
    "fload_3",
    "dload_0",
    "dload_1",
    "dload_2",
    "dload_3",
    "aload_0",
    "aload_1",
    "aload_2",
    "aload_3",
    "iaload",
    "laload",
    "faload",
    "daload",
    "aaload",
    "baload",
    "caload",
    "saload",
    "istore",
    "lstore",
    "fstore",
    "dstore",
    "astore",
    "istore_0",
    "istore_1",
    "istore_2",
    "istore_3",
    "lstore_0",
    "lstore_1",
    "lstore_2",
    "lstore_3",
    "fstore_0",
    "fstore_1",
    "fstore_2",
    "fstore_3",
    "dstore_0",
    "dstore_1",
    "dstore_2",
    "dstore_3",
    "astore_0",
    "astore_1",
    "astore_2",
    "astore_3",
    "iastore",
    "lastore",
    "fastore",
    "dastore",
    "aastore",
    "bastore",
    "castore",
    "sastore",
    "pop",
    "pop2",
    "dup",
    "dup_x1",
    "dup_x2",
    "dup2",
    "dup2_x1",
    "dup2_x2",
    "swap",
    "iadd",
    "ladd",
    "fadd",
    "dadd",
    "isub",
    "lsub",
    "fsub",
    "dsub",
    "imul",
    "lmul",
    "fmul",
    "dmul",
    "idiv",
    "ldiv",
    "fdiv",
    "ddiv",
    "irem",
    "lrem",
    "frem",
    "drem",
    "ineg",
    "lneg",
    "fneg",
    "dneg",
    "ishl",
    "lshl",
    "ishr",
    "lshr",
    "iushr",
    "lushr",
    "iand",
    "land",
    "ior",
    "lor",
    "ixor",
    "lxor",
    "iinc",
    "i2l",
    "i2f",
    "i2d",
    "l2i",
    "l2f",
    "l2d",
    "f2i",
    "f2l",
    "f2d",
    "d2i",
    "d2l",
    "d2f",
    "i2b",
    "i2c",
    "i2s",
    "lcmp",
    "fcmpl",
    "fcmpg",
    "dcmpl",
    "dcmpg",
    "ifeq",
    "ifne",
    "iflt",
    "ifge",
    "ifgt",
    "ifle",
    "if_icmpeq",
    "if_icmpne",
    "if_icmplt",
    "if_icmpge",
    "if_icmpgt",
    "if_icmple",
    "if_acmpeq",
    "if_acmpne",
    "goto",
    "jsr",
    "ret",
    "tableswitch",
    "lookupswitch",
    "ireturn",
    "lreturn",
    "freturn",
    "dreturn",
    "areturn",
    "return",
    "getstatic",
    "putstatic",
    "getfield",
    "putfield",
    "invokevirtual",
    "invokespecial",
    "invokestatic",
    "invokeinterface",
    "UNUSED",
    "new",
    "newarray",
    "anewarray",
    "arraylength",
    "athrow",
    "checkcast",
    "instanceof",
    "monitorenter",
    "monitorexit",
    "wide",
    "multianewarray",
    "ifnull",
    "ifnonnull",
    "goto_w",
    "jsr_w"
};

#endif
