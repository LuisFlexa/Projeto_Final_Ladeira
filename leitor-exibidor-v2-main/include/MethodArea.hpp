#ifndef METHOD_AREA_HPP
#define METHOD_AREA_HPP

/**
 * @file MethodArea.hpp
 * @brief Defines the MethodArea class responsible for managing class definitions in a JVM 8 environment.
 *
 * The MethodArea class implements a singleton pattern and stores loaded class definitions (StaticClass objects).
 * It provides functionality to load and retrieve class definitions by name, acting as a repository for
 * class metadata in the JVM runtime.
 */

#include "BasicTypes.hpp"
#include "StaticClass.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/**
 * @class MethodArea
 * @brief Manages class definitions in the JVM runtime.
 *
 * This class follows the singleton pattern to provide a single instance responsible for
 * loading and retrieving class definitions at runtime. It maintains a map of class names
 * to their corresponding StaticClass objects.
 */
class MethodArea {

public:
    /**
     * @brief Retrieves the singleton instance of the MethodArea.
     * @return A reference to the MethodArea instance.
     *
     * Ensures that only one instance of MethodArea exists.
     */
    static MethodArea& get_instance()
    {
        static MethodArea instance; ///< Singleton instance of the MethodArea
        return instance;
    }

    /**
     * @brief Destructor for MethodArea.
     */
    ~MethodArea();

    /**
     * @brief Loads a class by its name.
     * @param class_name The name of the class to load.
     * @return A pointer to the loaded StaticClass object.
     *
     * If the class is already loaded, it returns the existing instance.
     */
    StaticClass* load_class_by_name(const string& class_name);

    /**
     * @brief Retrieves a loaded class by its name.
     * @param class_name The name of the class.
     * @return A pointer to the StaticClass object, or nullptr if the class is not found.
     */
    StaticClass* get_class_by_name(const string& class_name);

    /**
     * @brief Path to the folder containing the necessary class files for the JVM execution
     */
    string class_path = "";

private:
    /**
     * @brief Private constructor to enforce singleton pattern.
     */
    MethodArea();

    /**
     * @brief Private copy constructor to prevent copying.
     */
    MethodArea(MethodArea const&);

    /**
     * @brief Private assignment operator to prevent assignment.
     */
    void operator=(MethodArea const&);

    /**
     * @brief Inserts a loaded class into the MethodArea.
     * @param class_file Pointer to the StaticClass object representing the class.
     * @return True if the class was successfully inserted, false otherwise.
     */
    bool insert_class(StaticClass* class_file);

    map<string, StaticClass*> classes; ///< Stores loaded class definitions.
};

#endif // METHOD_AREA_HPP
