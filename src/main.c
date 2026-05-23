#include "ClassFileReader.h"
#include "ClassFileViewer.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s -r=<class_file> [-o=<output_file>]\n", argv[0]);
        return 1;
    }

    char *class_file_path = NULL;
    char *output_file_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-r=", 3) == 0) {
            class_file_path = argv[i] + 3;
        } else if (strncmp(argv[i], "-o=", 3) == 0) {
            output_file_path = argv[i] + 3;
        }
    }

    if (!class_file_path) {
        printf("Error: Class file path is required. Use -r=<path>\n");
        return 1;
    }

    FILE *fp = fopen(class_file_path, "rb");
    if (!fp) {
        printf("Could not open file %s\n", class_file_path);
        return 1;
    }

    ClassFile *cf = read_class_file(fp);
    fclose(fp);

    if (cf) {
        FILE *out = stdout;
        if (output_file_path) {
            out = fopen(output_file_path, "w");
            if (!out) {
                printf("Could not open output file %s\n", output_file_path);
                out = stdout;
            }
        }
        
        print_class_file(cf, out);
        
        if (out != stdout) {
            fclose(out);
        }
        
        free_class_file(cf);
    }

    return 0;
}
