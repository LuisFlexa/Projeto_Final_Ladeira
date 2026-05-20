#include "ClassFileUtils.hpp"

using namespace std;

double ClassFileUtils::check_version(ClassFile* class_file)
{
    if (class_file->major_version < 45 || class_file->major_version > 52) {
        return 0;
    }
    if (class_file->major_version <= 48) {
        return 1.0 + (class_file->major_version - 44) * 0.1;
    }
    return 5 + (class_file->major_version - 49);
}

bool ClassFileUtils::check_this_class(string filename, string classname)
{
    int aux_pos;
    filename = filename.substr(0, filename.size() - 6);
    aux_pos = filename.find("\\");
    while (aux_pos >= 0 && (unsigned int)aux_pos <= filename.size()) {
        filename = filename.substr(aux_pos + 1);
        aux_pos = filename.find("\\");
    }
    aux_pos = filename.find("/");
    while (aux_pos >= 0 && (unsigned int)aux_pos <= filename.size()) {
        filename = filename.substr(aux_pos + 1);
        aux_pos = filename.find("/");
    }
    aux_pos = classname.find("\\");
    while (aux_pos >= 0 && (unsigned int)aux_pos <= classname.size()) {
        classname = classname.substr(aux_pos + 1);
        aux_pos = classname.find("\\");
    }
    aux_pos = classname.find("/");
    while (aux_pos >= 0 && (unsigned int)aux_pos <= classname.size()) {
        classname = classname.substr(aux_pos + 1);
        aux_pos = classname.find("/");
    }
    return (classname == filename);
}

double ClassFileUtils::verify_class_version(ClassFile* class_file)
{
    if (class_file->major_version < 45 || class_file->major_version > 52) {
        return 5 + (class_file->major_version - 49);
    }
    return 0;
}

bool ClassFileUtils::compare_utf8_str(ConstUtf8Info constant, const char* str)
{
    if (constant.length != strlen(str)) {
        return false;
    }
    for (int i = 0; i < constant.length; i++) {
        if (constant.bytes[i] != str[i])
            return false;
    }
    return true;
}

void ClassFileUtils::file_indent(FILE* out, uint8_t n)
{
    for (uint8_t i = 0; i < n; i++)
        fprintf(out, "\t");
}

const char* ClassFileUtils::stream_to_cstring(const stringstream& ss)
{
    string tmp = ss.str();
    char* result = (char*)malloc(sizeof(char) * (tmp.size() + 1));
    if (result == nullptr) {
        cerr << "Unable to malloc";
        exit(EXIT_FAILURE);
    }
    tmp.copy(result, tmp.size());
    result[tmp.size()] = '\0';
    return result;
}
