#ifndef DEMOS_H
#define DEMOS_H // Prevent this header being included twice in the same source file.
#include "CircularQueue.h"
#include "MinHeap.h"
#include "HashTable.h"
#include <string> // Text messages, number-to-text conversion and font file paths.
class Demos {
public:
    int mode = 0, step = 0;
    CircularQueue queue{Config::DemoCapacity};
    MinHeap heap{2};
    HashTable table{7};
    std::string event;
    void open(int which);
    void advance();
};

#endif
