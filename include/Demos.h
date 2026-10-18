#ifndef DEMOS_H
#define DEMOS_H // Prevent duplicate declarations when headers include each other.

#include "CircularQueue.h"
#include "MinHeap.h"
#include "HashTable.h"
#include <string> // Stores the operation message; to_string turns an ID into display text.

class Demos {
private:
    void queueStep();
    void heapStep();
    void hashStep();

public:
    int mode;
    int step;
    CircularQueue queue;
    MinHeap heap;
    HashTable table;
    std::string event;

    Demos();
    void open(int which);
    void advance();
};

#endif
