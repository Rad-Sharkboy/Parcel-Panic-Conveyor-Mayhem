#pragma once
#include "CircularQueue.h"
#include "MinHeap.h"
#include "HashTable.h"
#include <string>
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
