#pragma once
#include "Config.h"
class CircularQueue {
    int* values_; int front_ = 0, rear_ = 0, size_ = 0, capacity_;
public:
    explicit CircularQueue(int capacity = Config::InputCapacity);
    ~CircularQueue();
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;
    // Insert ID; false when full. No allocation, O(1).
    bool enqueue(int id);
    // Remove oldest ID into output; false when empty, O(1).
    bool dequeue(int& output);
    bool peek(int& output) const;
    void clear();
    int size() const { return size_; }
    int capacity() const { return capacity_; }
    int front() const { return front_; }
    int rear() const { return rear_; }
    int slot(int i) const { return values_[i]; }
};
