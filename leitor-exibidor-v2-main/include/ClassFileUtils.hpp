#ifndef CLASS_FILE_UTILS_HPP
#define CLASS_FILE_UTILS_HPP

#include "BasicTypes.hpp"
#include "ClassFileReader.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

using namespace std;

/**
 * @class ClassFileUtils
 * @brief A utility class that provides static methods for working with class files.
 *
 * This class contains various helper methods used to process and analyze class files, such as:
 * - Checking class file version
 * - Comparing UTF-8 strings
 * - Indentation handling for pretty printing
 * - Converting stringstreams to C-style strings
 * - Verifying the version of a given class file
 * - Checking if the class matches the given filename and class name
 */
class ClassFileUtils {
public:
    /**
     * @brief Checks the version of the given class file.
     *
     * @param class_file A pointer to a ClassFile structure.
     * @return A double representing the version of the class file.
     */
    static double check_version(ClassFile* class_file);

    /**
     * @brief Compares a UTF-8 constant from the constant pool with a given C-string.
     *
     * This function compares the string stored in a `ConstUtf8Info` object to a C-style string (`const char*`).
     *
     * @param constant The UTF-8 constant from the class file's constant pool.
     * @param str The C-style string to compare against.
     * @return True if the strings are equal, otherwise false.
     */
    static bool compare_utf8_str(ConstUtf8Info constant, const char* str);

    /**
     * @brief Adds indentation to the output stream.
     *
     * This function is typically used for pretty-printing class file data.
     * It writes `n` spaces to the provided output file.
     *
     * @param out The file pointer where indentation will be written.
     * @param n The number of spaces to indent.
     */
    static void file_indent(FILE* out, uint8_t n);

    /**
     * @brief Displays indentation on the standard output.
     *
     * This function is used for pretty-printing class file data to the console.
     * It prints `n` spaces to the standard output.
     *
     * @param n The number of spaces to indent.
     */
    static void display_indent(uint8_t n);

    /**
     * @brief Converts a stringstream to a C-style string.
     *
     * This function converts the contents of a `stringstream` object to a null-terminated C-string.
     *
     * @param ss The stringstream object containing the string data.
     * @return A C-style string containing the contents of the stringstream.
     */
    static const char* stream_to_cstring(const stringstream& ss);

    /**
     * @brief Verifies the version of the given class file.
     *
     * This function is similar to `check_version()` but may involve additional checks
     * for ensuring the version's correctness.
     *
     * @param class_file A pointer to a ClassFile structure.
     * @return A double representing the verified version of the class file.
     */
    static double verify_class_version(ClassFile* class_file);

    /**
     * @brief Checks if the class file's name matches the expected filename and class name.
     *
     * This function compares the class name in the class file with the specified class name
     * and checks if the class file corresponds to the given filename.
     *
     * @param filename The expected filename of the class file.
     * @param classname The expected class name.
     * @return True if the filename and class name match, otherwise false.
     */
    static bool check_this_class(string filename, string classname);
};

#endif
