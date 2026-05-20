#ifndef HEADER_FILE_NAME_HPP
#define HEADER_FILE_NAME_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold command-line argument values
typedef struct {
    char* class_file_path; ///< Path to the input class file
    char* output_file_path; ///< Path to the output file
    bool execute;
} Parameters;

/**
 * @brief Parses command-line arguments.
 *
 * This function processes the command-line arguments and returns a Parameters
 * structure containing the parsed values for the class file and output file paths.
 * It expects the class file path as the first argument and the output file path
 * as the second argument.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 *
 * @return A Parameters structure with class_file_path and output_file_path
 * set according to the arguments passed.
 */
const Parameters parse_args(int argc, char* argv[]);

/**
 * @brief Validates the provided parameters.
 *
 * This function checks if the given Parameters structure contains valid values
 * (non-null and meaningful parameters).
 *
 * @param params Pointer to a Parameters structure containing the paths to validate.
 *
 * @return true if the parameters are valid, false otherwise.
 */
bool validate_parameters(const Parameters* params);

#endif
