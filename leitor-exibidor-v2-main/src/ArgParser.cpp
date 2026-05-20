#include "ArgParser.hpp"
#include <iostream>

using namespace std;

const Parameters parse_args(int argc, char* argv[])
{
    Parameters params = { NULL, NULL };
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-r=", 3) == 0) {
            params.execute = false;
            params.class_file_path = argv[i] + 3;
        }
        else if (strncmp(argv[i], "-o=", 3) == 0)
            params.output_file_path = argv[i] + 3;
        else if (strncmp(argv[i], "-e=", 3) == 0) {
            params.execute = true;
            params.class_file_path = argv[i] + 3;
        }
    }
    return params;
}

bool validate_parameters(const Parameters* params)
{
    if (!params->class_file_path || (!params->execute && !params->output_file_path)) {
        cerr << "You need to call this binary passing either a file to read or to interpret\n";
        cerr << "Reader:\n\t./bin -r=class/file/Path.class -o=out.txt\n";
        cerr << "Interpreter:\n\t./bin -e=class/file/Path.class\n";
        return false;
    }
    return true;
}
