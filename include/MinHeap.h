#pragma once
#include "Config.h"
struct HeapEntry { int id = 0; int deadline = 0; unsigned long long sequence = 0; };
class MinHeap {
    HeapEntry* entries_; int size_ = 0, capacity_;
    static bool before(const HeapEntry& a, const HeapEntry& b);
public:
    explicit MinHeap(int capacity = Config::HeapCapacity);
    ~MinHeap();
    MinHeap(const MinHeap&) = delete;
    MinHeap& operator=(const MinHeap&) = delete;
    // Insert deadline/sequence entry, O(log n); O(n) when array grows.
    void insert(HeapEntry entry);
    // Extract earliest deadline, then earliest sequence. False if empty, O(log n).
    bool extract(HeapEntry& output);
    bool peek(HeapEntry& output) const;
    void clear() { size_ = 0; }
    int size() const { return size_; }
    int capacity() const { return capacity_; }
    const HeapEntry& at(int i) const { return entries_[i]; }
};
