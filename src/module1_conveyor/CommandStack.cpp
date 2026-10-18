#include "CommandStack.h"
CommandStack::~CommandStack() { clear(); }
void CommandStack::push(SwitchCommand command) { top_ = new Node{command, top_}; ++size_; }
bool CommandStack::pop(SwitchCommand& output) {
    if (!top_) return false;
    Node* old = top_; output = old->command; top_ = old->next; delete old; --size_; return true;
}
void CommandStack::clear() { SwitchCommand command{}; while (pop(command)) {} }
