#include "MinHeap.h"
#include <utility>
MinHeap::MinHeap(int capacity) : entries_(new HeapEntry[capacity > 0 ? capacity : 1]), capacity_(capacity > 0 ? capacity : 1) {}
MinHeap::~MinHeap() { delete[] entries_; }
bool MinHeap::before(const HeapEntry& a, const HeapEntry& b) {
    return a.deadline < b.deadline || (a.deadline == b.deadline && a.sequence < b.sequence);
}
void MinHeap::insert(HeapEntry entry) {
    if (size_ == capacity_) {
        int nextCapacity = capacity_ * 2;
        HeapEntry* next = new HeapEntry[nextCapacity];
        for (int i = 0; i < size_; ++i) next[i] = entries_[i];
        delete[] entries_; entries_ = next; capacity_ = nextCapacity;
    }
    int i = size_++; entries_[i] = entry;
    while (i > 0) { int parent = (i - 1) / 2; if (!before(entries_[i], entries_[parent])) break; std::swap(entries_[i], entries_[parent]); i = parent; }
}
bool MinHeap::extract(HeapEntry& output) {
    if (!size_) return false;
    output = entries_[0]; entries_[0] = entries_[--size_]; int i = 0;
    while (true) {
        int child = i * 2 + 1;
        if (child >= size_) break;
        if (child + 1 < size_ && before(entries_[child + 1], entries_[child])) ++child;
        if (!before(entries_[child], entries_[i])) break;
        std::swap(entries_[child], entries_[i]); i = child;
    }
    return true;
}
bool MinHeap::peek(HeapEntry& output) const { if (!size_) return false; output = entries_[0]; return true; }
