#include "Frame.hpp"
#include "ClassFileViewer.hpp"
#include "MethodArea.hpp"
#include "ClassFileUtils.hpp"

Frame::Frame(InstanceClass *object, StaticClass *classRuntime, string methodName, string methodDescriptor, vector<Value> arguments) :
    pc(0), object(object)
{

  for (int i = 0; i < (signed) arguments.size(); i++) {
    local_variables[i] = arguments[i];
  }

  MethodInfo *_method = obterMethodNamed(classRuntime, methodName, methodDescriptor);
  assert(_method != NULL);
  method = *_method;
  assert((method.access_flags & 0x0008) == 0); 

  encontrarAttributes();
}

Frame::Frame(StaticClass *classRuntime, string methodName, string methodDescriptor, vector<Value> arguments) :
    pc(0), object(NULL)
{

  for (int i = 0; i < (signed) arguments.size(); i++) {
    local_variables[i] = arguments[i];
  }

  MethodInfo *_method = obterMethodNamed(classRuntime, methodName, methodDescriptor);
  assert(_method != NULL);
  method = *_method;
  assert((method.access_flags & 0x0008) != 0); 

  encontrarAttributes();
}

Frame::~Frame()
{

}

ConstantPoolInfo** Frame::get_constant_pool()
{
  return &(class_runtime->get_class_file()->constant_pool);
}

Value Frame::get_local_variable_value(uint32_t index)
{
  if (index >= code_attribute->max_locals) {
    cerr << "Trying to get inexistent local variable" << endl;
    exit(1);
  }

  return local_variables[index];
}

void Frame::set_local_variable(Value variableValue, uint32_t index)
{
  if (index >= code_attribute->max_locals) {
    cerr << "Trying to set inexistent local variable" << endl;
    exit(1);
  }

  local_variables[index] = variableValue;
}

void Frame::push_operand_stack(Value operand)
{
  operand_stack.push(operand);
}

Value Frame::pop_operand_stack()
{
  if (operand_stack.size() == 0) {
    cerr << "IndexOutOfBoundsException" << endl;
    exit(1);
  }

  Value top = operand_stack.top();

  operand_stack.pop();
  return top;
}

stack<Value> Frame::copy_operand_stack()
{
  return operand_stack;
}

void Frame::load_operand_stack(stack<Value> backup)
{
  operand_stack = backup;
}

u1* Frame::get_code(uint32_t address)
{
  return code_attribute->code + address;
}

MethodInfo* Frame::obterMethodNamed(StaticClass *classRuntime, const string& name, const string& descriptor)
{
  MethodArea &methodArea = MethodArea::get_instance();

  StaticClass *currClass = classRuntime;
  MethodInfo *method;

  while (currClass != NULL) {
    ClassFile *classFile = currClass->get_class_file();

    for (int i = 0; i < classFile->methods_count; i++) {
     method = &(classFile->methods[i]);
     string methodName = get_formatted_constant(classFile->constant_pool, method->name_index);
     string methodDesc = get_formatted_constant(classFile->constant_pool, method->descriptor_index);

     if (methodName == name && methodDesc == descriptor) {
      class_runtime = currClass;
      return method;
     }
    }

    if (classFile->super_class == 0) {
     currClass = NULL;
    } else {
     string superClassName = get_formatted_constant(classFile->constant_pool, classFile->super_class);
     currClass = methodArea.get_class_by_name(superClassName);
    }
  }

  return NULL;
}

void Frame::encontrarAttributes()
{
  ConstantPoolInfo *constantPool = *get_constant_pool();

  code_attribute = NULL;
  exceptions_attribute = NULL;

  for (int i = 0; i < method.attributes_count; i++) {
    AttributeInfo *attr = &(method.attributes[i]);
    ConstUtf8Info attrName = constantPool[attr->attribute_name_index - 1].info.utf8_info;

    if (ClassFileUtils::compare_utf8_str(attrName, "Code")) {
     code_attribute = &(attr->info.code_info);
     if (exceptions_attribute != NULL)
      break;
    } else if (ClassFileUtils::compare_utf8_str(attrName, "Exceptions")) {
     exceptions_attribute = &(attr->info.exceptions_info);
     if (code_attribute != NULL)
      break;
    }
  }
}

u2 Frame::get_local_variables_vector_size()
{
  return code_attribute->max_locals;
}

u4 Frame::get_code_size()
{
  return code_attribute->code_length;
}
