#include "ClassFileViewer.h"
#include <math.h>

void print_constant_pool_info(ClassFile *cf, u2 index, FILE *out) {
    if (index >= cf->constant_pool_count || index == 0) return;
    cp_info *cp = &cf->constant_pool[index];
    switch (cp->tag) {
        case CONSTANT_Utf8:
            fprintf(out, "%s", cp->info.utf8_info.bytes);
            break;
        case CONSTANT_Class:
            print_constant_pool_info(cf, cp->info.class_info.name_index, out);
            break;
        case CONSTANT_String:
            print_constant_pool_info(cf, cp->info.string_info.string_index, out);
            break;
        case CONSTANT_NameAndType:
            print_constant_pool_info(cf, cp->info.name_and_type_info.name_index, out);
            fprintf(out, " ");
            print_constant_pool_info(cf, cp->info.name_and_type_info.descriptor_index, out);
            break;
        default:
            fprintf(out, "<unsupported string conversion>");
            break;
    }
}

void print_class_file(ClassFile *cf, FILE *out) {
    fprintf(out, "General Information\n{\n");
    fprintf(out, "\t Magic Number: \t\t\t 0x%08X\n", cf->magic);
    fprintf(out, "\t Minor Version: \t\t %d\n", cf->minor_version);
    
    float double_version = 0.0;
    if (cf->major_version >= 45) {
        double_version = 1.0 + (cf->major_version - 44) / 10.0;
    }
    fprintf(out, "\t Major Version: \t\t %d [%.1f]\n", cf->major_version, double_version);
    fprintf(out, "\t Constant Pool count: \t\t %d\n", cf->constant_pool_count);
    fprintf(out, "}\n\n");

    fprintf(out, "Constant Pool (Member count: %d)\n{\n", cf->constant_pool_count);
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cp_info *cp = &cf->constant_pool[i];
        switch (cp->tag) {
            case CONSTANT_Class:
                fprintf(out, "\t [%d] CONSTANT_Class_info\n", i);
                fprintf(out, "\t\t Class name: \t\t\t ConstantPoolInfo #%d <", cp->info.class_info.name_index);
                print_constant_pool_info(cf, cp->info.class_info.name_index, out);
                fprintf(out, ">\n");
                break;
            case CONSTANT_Fieldref:
                fprintf(out, "\t [%d] CONSTANT_Fieldref_info\n", i);
                fprintf(out, "\t\t Class name: \t\t\t ConstantPoolInfo #%d <", cp->info.fieldref_info.class_index);
                print_constant_pool_info(cf, cp->info.fieldref_info.class_index, out);
                fprintf(out, ">\n");
                fprintf(out, "\t\t Name and type: \t\t ConstantPoolInfo #%d <", cp->info.fieldref_info.name_and_type_index);
                print_constant_pool_info(cf, cp->info.fieldref_info.name_and_type_index, out);
                fprintf(out, ">\n");
                break;
            case CONSTANT_Methodref:
                fprintf(out, "\t [%d] CONSTANT_Methodref_info\n", i);
                fprintf(out, "\t\t Class name: \t\t\t ConstantPoolInfo #%d <", cp->info.methodref_info.class_index);
                print_constant_pool_info(cf, cp->info.methodref_info.class_index, out);
                fprintf(out, ">\n");
                fprintf(out, "\t\t Name and type: \t\t ConstantPoolInfo #%d <", cp->info.methodref_info.name_and_type_index);
                print_constant_pool_info(cf, cp->info.methodref_info.name_and_type_index, out);
                fprintf(out, ">\n");
                break;
            case CONSTANT_InterfaceMethodref:
                fprintf(out, "\t [%d] CONSTANT_InterfaceMethodref_info\n", i);
                fprintf(out, "\t\t Class name: \t\t\t ConstantPoolInfo #%d <", cp->info.interface_methodref_info.class_index);
                print_constant_pool_info(cf, cp->info.interface_methodref_info.class_index, out);
                fprintf(out, ">\n");
                fprintf(out, "\t\t Name and type: \t\t ConstantPoolInfo #%d <", cp->info.interface_methodref_info.name_and_type_index);
                print_constant_pool_info(cf, cp->info.interface_methodref_info.name_and_type_index, out);
                fprintf(out, ">\n");
                break;
            case CONSTANT_String:
                fprintf(out, "\t [%d] CONSTANT_String_info\n", i);
                fprintf(out, "\t\t String: \t\t\t ConstantPoolInfo #%d <", cp->info.string_info.string_index);
                print_constant_pool_info(cf, cp->info.string_info.string_index, out);
                fprintf(out, ">\n");
                break;
            case CONSTANT_Integer:
                fprintf(out, "\t [%d] CONSTANT_Integer_info\n", i);
                fprintf(out, "\t\t Bytes: \t\t\t 0x%08X\n", cp->info.integer_info.bytes);
                fprintf(out, "\t\t Integer: \t\t\t %d\n", (int32_t)cp->info.integer_info.bytes);
                break;
            case CONSTANT_Float:
                fprintf(out, "\t [%d] CONSTANT_Float_info\n", i);
                fprintf(out, "\t\t Bytes: \t\t\t 0x%08X\n", cp->info.float_info.bytes);
                float f;
                u4 bytes = cp->info.float_info.bytes;
                int32_t s = ((bytes >> 31) == 0) ? 1 : -1;
                int32_t e = ((bytes >> 23) & 0xff);
                int32_t m = (e == 0) ? (bytes & 0x7fffff) << 1 : (bytes & 0x7fffff) | 0x800000;
                f = s * m * pow(2, e - 150);
                fprintf(out, "\t\t Float: \t\t\t %f\n", f);
                break;
            case CONSTANT_Long:
                fprintf(out, "\t [%d] CONSTANT_Long_info\n", i);
                fprintf(out, "\t\t High Bytes: \t\t\t 0x%08X\n", cp->info.long_info.high_bytes);
                fprintf(out, "\t\t Low Bytes: \t\t\t 0x%08X\n", cp->info.long_info.low_bytes);
                int64_t l = ((int64_t)cp->info.long_info.high_bytes << 32) + cp->info.long_info.low_bytes;
                fprintf(out, "\t\t Long: \t\t\t\t %lld\n", (long long)l);
                break;
            case CONSTANT_Double:
                fprintf(out, "\t [%d] CONSTANT_Double_info\n", i);
                fprintf(out, "\t\t High Bytes: \t\t\t 0x%08X\n", cp->info.double_info.high_bytes);
                fprintf(out, "\t\t Low Bytes: \t\t\t 0x%08X\n", cp->info.double_info.low_bytes);
                int64_t d_bytes = ((int64_t)cp->info.double_info.high_bytes << 32) + cp->info.double_info.low_bytes;
                int32_t d_s = ((d_bytes >> 63) == 0) ? 1 : -1;
                int32_t d_e = (int32_t)((d_bytes >> 52) & 0x7ffL);
                int64_t d_m = (d_e == 0) ? (d_bytes & 0xfffffffffffffL) << 1 : (d_bytes & 0xfffffffffffffL) | 0x10000000000000L;
                double d = d_s * d_m * pow(2, d_e - 1075);
                fprintf(out, "\t\t Double: \t\t\t %f\n", d);
                break;
            case 0:
                fprintf(out, "\t [%d] (large numeric continued)\n", i);
                break;
            case CONSTANT_NameAndType:
                fprintf(out, "\t [%d] CONSTANT_NameAndType_info\n", i);
                fprintf(out, "\t\t Name: \t\t\t\t ConstantPoolInfo #%d <", cp->info.name_and_type_info.name_index);
                print_constant_pool_info(cf, cp->info.name_and_type_info.name_index, out);
                fprintf(out, ">\n");
                fprintf(out, "\t\t Descriptor: \t\t\t ConstantPoolInfo #%d <", cp->info.name_and_type_info.descriptor_index);
                print_constant_pool_info(cf, cp->info.name_and_type_info.descriptor_index, out);
                fprintf(out, ">\n");
                break;
            case CONSTANT_Utf8:
                fprintf(out, "\t [%d] CONSTANT_Utf8_info\n", i);
                fprintf(out, "\t\t Length of byte array: \t\t %d\n", cp->info.utf8_info.length);
                fprintf(out, "\t\t Length of string: \t\t %d\n", cp->info.utf8_info.length);
                fprintf(out, "\t\t String: \t\t\t %s\n", cp->info.utf8_info.bytes);
                break;
        }
        fprintf(out, "\n");
    }
    fprintf(out, "}\n");
}
