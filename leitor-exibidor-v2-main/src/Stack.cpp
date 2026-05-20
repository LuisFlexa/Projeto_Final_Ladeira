#include "Stack.hpp"

Stack::Stack()
{

}

Stack::~Stack()
{

}

void Stack::push_frame(Frame *frame)
{
  // Verifica se a stack está cheia
//    if (frame_stack.size() >= FRAME_MAX_SIZE) {
//        cerr << "StackOverflowError" << endl;
//        exit(1);
//    }

  frame_stack.push(frame);
}

Frame* Stack::get_top_frame()
{
  if (frame_stack.size() == 0) {
    return NULL;
  }

  return frame_stack.top();
}

bool Stack::pop_frame()
{
  if (frame_stack.size() == 0) {
    return false;
  }

  Frame *frame = frame_stack.top();
  frame_stack.pop();
  delete frame;

  return true;
}

uint32_t Stack::size()
{
  return frame_stack.size();
}
