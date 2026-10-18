#include "CommandStack.h"

CommandStack::CommandStack() {
    top = nullptr;
    count = 0;
}

CommandStack::~CommandStack() {
    clear();
}

bool CommandStack::isEmpty() const {
    return top == nullptr;
}

void CommandStack::push(SwitchCommand command) {
    StackNode* temp = new StackNode;
    temp->data = command;
    temp->next = top;
    top = temp;
    count++;
}

SwitchCommand CommandStack::pop() {
    if (isEmpty()) {
        SwitchCommand empty;
        return empty;
    }
    StackNode* temp = top;
    SwitchCommand command = temp->data;
    top = top->next;
    delete temp;
    count--;
    return command;
}

void CommandStack::clear() {
    while (!isEmpty()) {
        pop();
    }
}

void CommandStack::copyFrom(const CommandStack& other) {
    // Append copied nodes so their top-to-bottom order stays the same.
    const StackNode* current = other.top;
    StackNode* tail = nullptr;
    while (current != nullptr) {
        StackNode* temp = new StackNode;
        temp->data = current->data;
        temp->next = nullptr;
        if (top == nullptr) {
            top = temp;
        } else {
            tail->next = temp;
        }
        tail = temp;
        current = current->next;
        count++;
    }
}

CommandStack::CommandStack(const CommandStack& other) {
    top = nullptr;
    count = 0;
    copyFrom(other);
}

CommandStack& CommandStack::operator=(const CommandStack& other) {
    if (this == &other) {
        return *this;
    }
    clear();
    copyFrom(other);
    return *this;
}
