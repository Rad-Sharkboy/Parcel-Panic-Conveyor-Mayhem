#include "MinHeap.h"

MinHeap::MinHeap(int initialCapacity) {
    if (initialCapacity < 1) {
        initialCapacity = 1;
    }
    capacity = initialCapacity;
    count = 0;
    heap = new HeapEntry[capacity];
}

MinHeap::~MinHeap() {
    delete[] heap;
}

MinHeap::MinHeap(const MinHeap& other) {
    capacity = other.capacity;
    count = other.count;
    heap = new HeapEntry[capacity];
    for (int i = 0; i < count; i++) {
        heap[i] = other.heap[i];
    }
}

MinHeap& MinHeap::operator=(const MinHeap& other) {
    if (this == &other) {
        return *this;
    }
    delete[] heap;
    capacity = other.capacity;
    count = other.count;
    heap = new HeapEntry[capacity];
    for (int i = 0; i < count; i++) {
        heap[i] = other.heap[i];
    }
    return *this;
}

bool MinHeap::isEmpty() const {
    return count == 0;
}

// Primary key: earliest deadline first; tie-breaker: lower arrival sequence first (FIFO).
bool MinHeap::comesBefore(HeapEntry first, HeapEntry second) const {
    if (first.deadline == second.deadline) {
        return first.sequence < second.sequence;
    }
    return first.deadline < second.deadline;
}

// Restores min-heap property upward from a newly inserted leaf node in O(log n).
void MinHeap::siftUp(int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (!comesBefore(heap[index], heap[parentIndex])) {
            break;
        }
        HeapEntry temp = heap[index];
        heap[index] = heap[parentIndex];
        heap[parentIndex] = temp;
        index = parentIndex;
    }
}

// Restores min-heap property downward from root after extraction in O(log n).
void MinHeap::siftDown(int index) {
    while (true) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        if (leftChild >= count) {
            break; // Leaf reached: heap order satisfied.
        }
        int highestPriorityChild = leftChild;
        if (rightChild < count && comesBefore(heap[rightChild], heap[leftChild])) {
            highestPriorityChild = rightChild;
        }
        if (!comesBefore(heap[highestPriorityChild], heap[index])) {
            break;
        }
        HeapEntry temp = heap[index];
        heap[index] = heap[highestPriorityChild];
        heap[highestPriorityChild] = temp;
        index = highestPriorityChild;
    }
}

void MinHeap::insert(HeapEntry entry) {
    if (count == capacity) {
        int expandedCapacity = capacity * 2;
        HeapEntry* resizedHeap = new HeapEntry[expandedCapacity];
        for (int i = 0; i < count; i++) {
            resizedHeap[i] = heap[i];
        }
        delete[] heap;
        heap = resizedHeap;
        capacity = expandedCapacity;
    }

    int insertIndex = count;
    heap[insertIndex] = entry;
    count++;
    siftUp(insertIndex);
}

bool MinHeap::extract(HeapEntry& output) {
    if (isEmpty()) {
        return false;
    }
    output = heap[0];
    count--;
    if (count > 0) {
        heap[0] = heap[count];
        siftDown(0);
    }
    return true;
}

bool MinHeap::peek(HeapEntry& output) const {
    if (isEmpty()) {
        return false;
    }
    output = heap[0];
    return true;
}

void MinHeap::clear() {
    count = 0;
}