#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H // Prevent duplicate declarations when headers include each other.

#include "Config.h"

// Same circular-array pattern as the field manual. Only parcel IDs go in this queue.
class CircularQueue {
private:
    int* arr;
    int front;
    int rear;
    int capacity;
    int count;

public:
    CircularQueue(int capacity = Config::InputCapacity);
    ~CircularQueue();

    // Lab 01: deep copies give each queue its own array instead of sharing a pointer.
    CircularQueue(const CircularQueue& other);
    CircularQueue& operator=(const CircularQueue& other);

    bool isEmpty() const;
    bool isFull() const;
    // Add an ID at rear. Returns false if full. O(1).
    bool enqueue(int id);
    // Remove/return the front ID, or -1 if empty. O(1).
    int dequeue();
    // Read the front ID without removing it, or -1 if empty. O(1).
    int peek() const;
    void clear();

    int size() const { return count; }
    int getCapacity() const { return capacity; }
    int getFront() const { return front; }
    int getRear() const { return rear; }
    int getSlot(int index) const { return arr[index]; }
};

#endif
