#ifndef COMMANDSTACK_H
#define COMMANDSTACK_H // Prevent duplicate declarations when headers include each other.

struct SwitchCommand {
    int cell = -1;
    int oldChoice = 0;
};

struct StackNode {
    SwitchCommand data;
    StackNode* next;
};

// A linked stack: push inserts at the head; pop removes the head (Lab 05).
class CommandStack {
private:
    StackNode* top;
    int count;
    void copyFrom(const CommandStack& other);

public:
    CommandStack();
    ~CommandStack();
    CommandStack(const CommandStack& other);
    CommandStack& operator=(const CommandStack& other);

    bool isEmpty() const;
    // Save a switch's old choice at the top. O(1).
    void push(SwitchCommand command);
    // Remove/return the newest command. cell is -1 if empty. O(1).
    SwitchCommand pop();
    void clear();
    int size() const { return count; }
};

#endif
