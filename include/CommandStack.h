#pragma once
struct SwitchCommand { int cell; int oldChoice; };
class CommandStack {
    struct Node { SwitchCommand command; Node* next; };
    Node* top_ = nullptr; int size_ = 0;
public:
    CommandStack() = default;
    ~CommandStack();
    CommandStack(const CommandStack&) = delete;
    CommandStack& operator=(const CommandStack&) = delete;
    // Record reversible planning edit in O(1).
    void push(SwitchCommand command);
    // Restore most recent edit into output; false if empty, O(1).
    bool pop(SwitchCommand& output);
    void clear();
    int size() const { return size_; }
};
