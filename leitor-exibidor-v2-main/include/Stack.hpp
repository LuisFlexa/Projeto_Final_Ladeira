#ifndef STACK_HPP
#define STACK_HPP

#include "Frame.hpp"
#include <cstdlib>
#include <iostream>
#include <stack>

using namespace std;

/**
 * @brief Maximum number of frames allowed on the JVM call stack.
 *
 * Although defined here, the actual enforcement of this limit (FRAME_MAX_SIZE)
 * should be implemented in the corresponding source file. This constant ensures
 * that the JVM does not exceed its stack capacity, which would otherwise result in
 * a StackOverflowError as per the JVM specification.
 */
#define FRAME_MAX_SIZE 50

/**
 * @class Stack
 * @brief Singleton class representing the call stack of a JVM.
 *
 * In the context of a JVM 8 implementation, the Stack class maintains a stack
 * of method invocation frames (each represented by a Frame pointer). Every time
 * a method is called, a new frame is pushed onto the stack. When a method call
 * completes, its corresponding frame is popped from the stack. This behavior is
 * essential for managing method calls, local variables, operand stacks, and the
 * overall execution state in a Java Virtual Machine.
 *
 * This class is implemented using the Singleton pattern to ensure that there is
 * only one call stack per JVM instance. The static method get_instance() provides
 * global access to the unique Stack instance.
 */
class Stack {

public:
    /**
     * @brief Returns the singleton instance of the Stack.
     *
     * This static method guarantees that only one instance of the Stack exists.
     * It uses a local static variable to instantiate the object on first use,
     * following the Meyers Singleton pattern.
     *
     * @return A reference to the unique Stack instance.
     */
    static Stack& get_instance()
    {
        static Stack instance;
        return instance;
    }

    /**
     * @brief Destructor.
     *
     * Responsible for cleaning up the stack. This may involve releasing
     * dynamically allocated resources held by the frames. The exact cleanup
     * behavior should be defined in the corresponding implementation file.
     */
    ~Stack();

    /**
     * @brief Pushes a new frame onto the call stack.
     *
     * When a Java method is invoked, a corresponding Frame is created to hold
     * its local variables and operand stack. This method adds the frame to the
     * top of the call stack.
     *
     * @param frame Pointer to the Frame object representing the new method invocation.
     */
    void push_frame(Frame* frame);

    /**
     * @brief Retrieves the frame at the top of the call stack.
     *
     * The top frame represents the currently executing method's frame. This method
     * returns a pointer to that frame, allowing the JVM to access local variables,
     * operand stacks, and other execution context details.
     *
     * @return Pointer to the top Frame on the stack. Returns nullptr if the stack is empty.
     */
    Frame* get_top_frame();

    /**
     * @brief Removes the frame at the top of the call stack.
     *
     * When a method call completes, its corresponding frame should be removed from
     * the call stack. This method performs that operation. It returns a boolean value
     * indicating whether the operation was successful (i.e., whether there was a frame
     * to pop).
     *
     * @return True if a frame was successfully popped; false if the stack was empty.
     */
    bool pop_frame();

    /**
     * @brief Retrieves the current number of frames on the call stack.
     *
     * This method is useful for debugging and monitoring the state of the JVM,
     * as it indicates how deep the method invocation stack currently is.
     *
     * @return The number of Frame pointers stored in the stack.
     */
    uint32_t size();

private:
    /**
     * @brief Private constructor to enforce singleton pattern.
     *
     * The constructor is private to prevent direct instantiation of the Stack class.
     * Clients must use get_instance() to access the singleton instance.
     */
    Stack();

    /**
     * @brief Copy constructor is disabled.
     *
     * Declared privately and not defined to prevent copying of the singleton instance.
     *
     * @param other Reference to another Stack instance (unused).
     */
    Stack(Stack const&);

    /**
     * @brief Assignment operator is disabled.
     *
     * Declared privately and not defined to prevent assignment between instances of
     * the Stack singleton.
     *
     * @param other Reference to another Stack instance (unused).
     */
    void operator=(Stack const&);

    /**
     * @brief The underlying container that holds the Frame pointers.
     *
     * The standard template library (STL) stack is used to manage the call frames.
     * Each frame corresponds to a Java method invocation and stores its execution
     * context.
     */
    stack<Frame*> frame_stack;
};

#endif
