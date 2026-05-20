/*
 * Frame.hpp
 *
 * This header file defines the Frame class, which represents a stack frame in the JVM 8 runtime environment.
 * A Frame encapsulates execution state, including local variables, the operand stack, and method metadata.
 *
 * Dependencies:
 * - BasicTypes.hpp: Defines fundamental JVM value types.
 * - InstanceClass.hpp: Represents an instance of a class.
 * - StaticClass.hpp: Represents a static class runtime structure.
 * - <map>, <stack>, <vector>: Used for data management.
 * - <cassert>, <cstdlib>, <cstring>, <iostream>: Standard C++ utilities.
 *
 * Author: [Your Name]
 * Date: [Date]
 */

#ifndef FRAME_HPP
#define FRAME_HPP

#include "BasicTypes.hpp"
#include "InstanceClass.hpp"
#include "StaticClass.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <stack>
#include <vector>

using namespace std;

/**
 * @class Frame
 * @brief Represents a JVM stack frame used in method execution.
 */
class Frame {

public:
    /**
     * @brief Constructs a Frame for an instance method.
     * @param object The object instance associated with the method.
     * @param class_runtime The static class runtime.
     * @param method_name The name of the method.
     * @param method_descriptor The method descriptor.
     * @param arguments The arguments passed to the method.
     */
    Frame(InstanceClass* object, StaticClass* class_runtime, string method_name, string method_descriptor, vector<Value> arguments);

    /**
     * @brief Constructs a Frame for a static method.
     * @param class_runtime The static class runtime.
     * @param method_name The name of the method.
     * @param method_descriptor The method descriptor.
     * @param arguments The arguments passed to the method (default empty vector).
     */
    Frame(StaticClass* class_runtime, string method_name, string method_descriptor, vector<Value> arguments = vector<Value>());

    /**
     * @brief Destructor to clean up resources.
     */
    ~Frame();

    /**
     * @brief Retrieves the constant pool.
     * @return A pointer to the constant pool.
     */
    ConstantPoolInfo** get_constant_pool();

    /**
     * @brief Gets a local variable value by index.
     * @param index The index of the local variable.
     * @return The value of the local variable.
     */
    Value get_local_variable_value(uint32_t index);

    /**
     * @brief Sets a local variable value at a given index.
     * @param variableValue The value to store.
     * @param index The index of the variable.
     */
    void set_local_variable(Value variableValue, uint32_t index);

    /**
     * @brief Pushes a value onto the operand stack.
     * @param operand The value to push.
     */
    void push_operand_stack(Value operand);

    /**
     * @brief Pops a value from the operand stack.
     * @return The popped value.
     */
    Value pop_operand_stack();

    /**
     * @brief Creates a copy of the operand stack.
     * @return A copy of the operand stack.
     */
    stack<Value> copy_operand_stack();

    /**
     * @brief Loads a backup operand stack into the frame.
     * @param backup The operand stack backup to restore.
     */
    void load_operand_stack(stack<Value> backup);

    /**
     * @brief Gets the object associated with this frame.
     * @return A pointer to the InstanceClass object.
     */
    InstanceClass* get_object();

    /**
     * @brief Retrieves the bytecode instruction at a specific address.
     * @param address The bytecode address.
     * @return A pointer to the instruction.
     */
    u1* get_code(uint32_t address);

    /**
     * @brief The program counter for this frame.
     */
    u4 pc;

    /**
     * @brief Gets the size of the local variables vector.
     * @return The size of the local variables vector.
     */
    u2 get_local_variables_vector_size();

    /**
     * @brief Gets the total size of the method bytecode.
     * @return The size of the method's code attribute.
     */
    u4 get_code_size();

private:
    /**
     * @brief Finds a method by name and descriptor in a given class runtime.
     * @param class_runtime The static class runtime.
     * @param name The method name.
     * @param descriptor The method descriptor.
     * @return A pointer to the located MethodInfo.
     */
    MethodInfo* obterMethodNamed(StaticClass* class_runtime, const string& name, const string& descriptor);

    /**
     * @brief Finds and initializes method attributes.
     */
    void encontrarAttributes();

    StaticClass* class_runtime; ///< Pointer to the static class runtime.
    InstanceClass* object; ///< Pointer to the instance object (if applicable).
    MethodInfo method; ///< Method information for this frame.
    CodeAttribute* code_attribute; ///< Pointer to the method's code attribute.
    ExceptionsAttribute* exceptions_attribute; ///< Pointer to the method's exceptions attribute.
    map<uint32_t, Value> local_variables; ///< Map storing local variables by index.
    stack<Value> operand_stack; ///< Operand stack for the frame.
};

#endif // FRAME_HPP