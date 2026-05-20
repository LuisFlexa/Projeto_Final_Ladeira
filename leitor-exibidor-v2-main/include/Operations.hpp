#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include "Array.hpp"
#include "BasicTypes.hpp"
#include "ClassFileViewer.hpp"
#include "Frame.hpp"
#include "InstanceClass.hpp"
#include "MethodArea.hpp"
#include "Stack.hpp"
#include "StaticClass.hpp"
#include "StrObject.hpp"
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stack>
#include <string>

using namespace std;

class Operations;
typedef void (Operations::*generic_function)();

/**
 * @brief Represents the core operations used in the JVM 8 implementation.
 *
 * The Operations class acts as the central executor for various JVM-level operations.
 * It is implemented as a singleton to ensure that a single, consistent set of operations
 * is used throughout the virtual machine. In addition to maintaining an array of function
 * pointers for the individual bytecode instructions (e.g., nop, iconst_0, etc.), the core
 * functions provided by this class facilitate:
 *
 *  - Executing methods defined in a static class.
 *  - Verifying the existence of a specific method (by name and descriptor) within a class.
 *  - Populating multi-dimensional arrays.
 *
 * For the purposes of this documentation, only the high-level core functions are described.
 * The instruction-specific functions (e.g., nop, bipush, ldc, etc.) are part of the internal
 * implementation and are not covered here.
 */
class Operations {
public:
    /**
     * @brief Retrieves the singleton instance of the Operations class.
     *
     * This static method returns the single, global instance of Operations. The singleton
     * design ensures that all components of the JVM use a consistent set of operation routines.
     *
     * @return A reference to the unique Operations instance.
     */
    static Operations& get_instance()
    {
        static Operations instance;
        return instance;
    }

    /**
     * @brief Destructor for the Operations class.
     *
     * Cleans up any internal resources associated with the Operations instance. Although most
     * resources are managed automatically, this destructor provides a hook for explicit cleanup if
     * required.
     */
    ~Operations();

    /**
     * @brief Executes methods associated with a given static class.
     *
     * In the JVM, classes may contain static methods (such as static initializers) that need to be
     * executed during class loading or initialization. This function triggers the execution of such
     * methods for the provided static class.
     *
     * @param class_runtime A pointer to the static class whose methods are to be executed.
     */
    void executeMethods(StaticClass* class_runtime);

    /**
     * @brief Verifies whether a method exists within a static class.
     *
     * This function checks for the existence of a method with a given name and descriptor within
     * the provided static class. The descriptor encodes the method’s signature (parameter types
     * and return type) and is used along with the method name to uniquely identify methods in the JVM.
     *
     * @param class_runtime A pointer to the static class in which to search for the method.
     * @param name The name of the method to locate.
     * @param descriptor The descriptor (signature) of the method.
     * @return True if the method exists in the class; false otherwise.
     */
    bool verifyMethod(StaticClass* class_runtime, const string& name, const string& descriptor);

    /**
     * @brief Populates a multi-dimensional array with initial values.
     *
     * In Java, multi-dimensional arrays are implemented as arrays of arrays. This function
     * recursively populates such an array based on the dimensions specified in the 'count' stack.
     * The parameter 'value' represents the type of elements that the array should hold, guiding
     * the initialization process.
     *
     * @param array A pointer to the Array object representing the multi-dimensional array.
     * @param value The ValueType indicating the type of the array's elements.
     * @param count A stack of integers where each element represents the size of a dimension.
     */
    void populateMultiarray(Array* array, ValueType value, stack<int> count);

private:
    /**
     * @brief Private constructor to enforce the singleton pattern.
     *
     * This constructor is private to prevent direct instantiation of the Operations class.
     * Clients must use get_instance() to obtain the single available instance.
     */
    Operations();

    /**
     * @brief Disabled copy constructor.
     *
     * The copy constructor is declared but not defined to prevent copying of the Operations instance.
     *
     * @param other Another Operations instance (unused).
     */
    Operations(Operations const&);

    /**
     * @brief Disabled assignment operator.
     *
     * The assignment operator is declared but not defined to prevent assignment between
     * Operations instances.
     *
     * @param other Another Operations instance (unused).
     */
    void operator=(Operations const&);

    /**
     * @brief Array of generic function pointers for JVM instruction implementations.
     *
     * This array holds pointers to member functions that implement the various bytecode
     * instructions of the JVM. Although there are many instruction functions (e.g., nop, iconst_0,
     * bipush, etc.), these lower-level operations are not the focus of the core functionality
     * documented here.
     */
    generic_function generic_functions_arr[202];

    /**
     * @brief Flag indicating if the current instruction is in wide mode.
     *
     * Certain JVM instructions operate in a "wide" mode that affects the size of their operands.
     * This boolean flag is used internally to track whether wide mode is active.
     */
    bool is_wide;

    // Instruction functions (e.g., nop, iconst_m1, iconst_0, etc.) are implemented below.
    // These functions provide the low-level behavior of the JVM instructions and are omitted
    // from this documentation.
    void nop();

    void aconst_null();

    void iconst_m1();

    void iconst_0();

    void iconst_1();

    void iconst_2();

    void iconst_3();

    void iconst_4();

    void iconst_5();

    void lconst_0();

    void lconst_1();

    void fconst_0();

    void fconst_1();

    void fconst_2();

    void dconst_0();

    void dconst_1();

    void bipush();

    void sipush();

    void ldc();

    void ldc_w();

    void ldc2_w();

    void iload();

    void lload();

    void fload();

    void dload();

    void aload();

    void iload_0();

    void iload_1();

    void iload_2();

    void iload_3();

    void lload_0();

    void lload_1();

    void lload_2();

    void lload_3();

    void fload_0();

    void fload_1();

    void fload_2();

    void fload_3();

    void dload_0();

    void dload_1();

    void dload_2();

    void dload_3();

    void aload_0();

    void aload_1();

    void aload_2();

    void aload_3();

    void iaload();

    void laload();

    void faload();

    void daload();

    void aaload();

    void baload();

    void caload();

    void saload();

    void istore();

    void lstore();

    void fstore();

    void dstore();

    void astore();

    void istore_0();

    void istore_1();

    void istore_2();

    void istore_3();

    void lstore_0();

    void lstore_1();

    void lstore_2();

    void lstore_3();

    void fstore_0();

    void fstore_1();

    void fstore_2();

    void fstore_3();

    void dstore_0();

    void dstore_1();

    void dstore_2();

    void dstore_3();

    void astore_0();

    void astore_1();

    void astore_2();

    void astore_3();

    void iastore();

    void lastore();

    void fastore();

    void dastore();

    void aastore();

    void bastore();

    void castore();

    void sastore();

    void pop();

    void pop2();

    void dup();

    void dup_x1();

    void dup_x2();

    void dup2();

    void dup2_x1();

    void dup2_x2();

    void swap();

    void iadd();

    void ladd();

    void fadd();

    void dadd();

    void isub();

    void lsub();

    void fsub();

    void dsub();

    void imul();

    void lmul();

    void fmul();

    void dmul();

    void idiv();

    void ldiv();

    void fdiv();

    void ddiv();

    void irem();

    void lrem();

    void frem();

    void drem();

    void ineg();

    void lneg();

    void fneg();

    void dneg();

    void ishl();

    void lshl();

    void ishr();

    void lshr();

    void iushr();

    void lushr();

    void iand();

    void land();

    void ior();

    void lor();

    void ixor();

    void lxor();

    void iinc();

    void i2l();

    void i2f();

    void i2d();

    void l2i();

    void l2f();

    void l2d();

    void f2i();

    void f2l();

    void f2d();

    void d2i();

    void d2l();

    void d2f();

    void i2b();

    void i2c();

    void i2s();

    void lcmp();

    void fcmpl();

    void fcmpg();

    void dcmpl();

    void dcmpg();

    void ifeq();

    void ifne();

    void iflt();

    void ifge();

    void ifgt();

    void ifle();

    void if_icmpeq();

    void if_icmpne();

    void if_icmplt();

    void if_icmpge();

    void if_icmpgt();

    void if_icmple();

    void if_acmpeq();

    void if_acmpne();

    void func_goto();

    void jsr();

    void ret();

    void tableswitch();

    void lookupswitch();

    void ireturn();

    void lreturn();

    void freturn();

    void dreturn();

    void areturn();

    void func_return();

    void getstatic();

    void putstatic();

    void getfield();

    void putfield();

    void invokevirtual();

    void invokespecial();

    void invokestatic();

    void invokeinterface();

    void func_new();

    void newarray();

    void anewarray();

    void arraylength();

    void athrow();

    void checkcast();

    void instanceof();

    void monitorenter();

    void monitorexit();

    void wide();

    void multianewarray();

    void ifnull();

    void ifnonnull();

    void goto_w();

    void jsr_w();

    void initInstructions();
};

#endif
