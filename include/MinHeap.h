#ifndef MINHEAP_H
#define MINHEAP_H // Prevent duplicate declarations when headers include each other.

#include "Config.h"

struct HeapEntry {
    int id = 0;
    int deadline = 0;
    int sequence = 0; // Arrival number: breaks ties between equal deadlines.
};

class MinHeap {
private:
    HeapEntry* heap;
    int count;
    int capacity;
    bool comesBefore(HeapEntry first, HeapEntry second) const;

public:
    MinHeap(int capacity = Config::HeapCapacity);
    ~MinHeap();
    MinHeap(const MinHeap& other);
    MinHeap& operator=(const MinHeap& other);

    bool isEmpty() const;
    // Add an entry and move it toward the root. O(log n), or O(n) if the array grows.
    void insert(HeapEntry entry);
    // Remove the root. 'output' receives the entry; false means empty. O(log n).
    bool extract(HeapEntry& output);
    // Read the root into 'output' without removing it. O(1).
    bool peek(HeapEntry& output) const;
    void clear();
    int size() const { return count; }
    int getCapacity() const { return capacity; }
    const HeapEntry& getEntry(int index) const { return heap[index]; }
};

#endif
