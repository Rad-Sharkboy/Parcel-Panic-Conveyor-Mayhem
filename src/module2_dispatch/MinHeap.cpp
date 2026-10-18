#include "MinHeap.h"

MinHeap::MinHeap(int size) {
    if (size < 1) {
        size = 1;
    }
    capacity = size;
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

bool MinHeap::comesBefore(HeapEntry first, HeapEntry second) const {
    if (first.deadline == second.deadline) {
        return first.sequence < second.sequence;
    }
    return first.deadline < second.deadline;
}

void MinHeap::insert(HeapEntry entry) {
    if (count == capacity) {
        int newCapacity = capacity * 2;
        HeapEntry* temp = new HeapEntry[newCapacity];
        for (int i = 0; i < count; i++) {
            temp[i] = heap[i];
        }
        delete[] heap;
        heap = temp;
        capacity = newCapacity;
    }

    int index = count;
    heap[index] = entry;
    count++;

    // In an array heap, the parent of index i is (i - 1) / 2.
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (!comesBefore(heap[index], heap[parent])) {
            break;
        }
        HeapEntry temp = heap[index];
        heap[index] = heap[parent];
        heap[parent] = temp;
        index = parent;
    }
}

bool MinHeap::extract(HeapEntry& output) {
    if (isEmpty()) {
        return false;
    }
    output = heap[0];
    count--;
    heap[0] = heap[count]; // Put the last item at the root, then move it down.
    int index = 0;

    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        if (left >= count) {
            break; // No children: this entry is already in place.
        }
        int smaller = left;
        if (right < count && comesBefore(heap[right], heap[left])) {
            smaller = right;
        }
        if (!comesBefore(heap[smaller], heap[index])) {
            break;
        }
        HeapEntry temp = heap[index];
        heap[index] = heap[smaller];
        heap[smaller] = temp;
        index = smaller;
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
