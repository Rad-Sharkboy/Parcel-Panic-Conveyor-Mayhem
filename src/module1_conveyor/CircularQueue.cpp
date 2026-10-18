#include "CircularQueue.h"

CircularQueue::CircularQueue(int size) {
    if (size < 1) {
        size = 1;
    }
    capacity = size;
    arr = new int[capacity];
    clear();
}

CircularQueue::~CircularQueue() {
    delete[] arr;
}

CircularQueue::CircularQueue(const CircularQueue& other) {
    capacity = other.capacity;
    front = other.front;
    rear = other.rear;
    count = other.count;
    arr = new int[capacity];
    for (int i = 0; i < capacity; i++) {
        arr[i] = other.arr[i];
    }
}

CircularQueue& CircularQueue::operator=(const CircularQueue& other) {
    if (this == &other) {
        return *this; // Assigning a queue to itself needs no work.
    }
    delete[] arr;
    capacity = other.capacity;
    front = other.front;
    rear = other.rear;
    count = other.count;
    arr = new int[capacity];
    for (int i = 0; i < capacity; i++) {
        arr[i] = other.arr[i];
    }
    return *this;
}

bool CircularQueue::isEmpty() const {
    return count == 0;
}

bool CircularQueue::isFull() const {
    return count == capacity;
}

bool CircularQueue::enqueue(int id) {
    if (isFull()) {
        return false;
    }
    rear = (rear + 1) % capacity; // Wrap to slot 0 after the last slot.
    arr[rear] = id;
    count++;
    return true;
}

int CircularQueue::dequeue() {
    if (isEmpty()) {
        return -1;
    }
    int id = arr[front];
    arr[front] = 0; // Clear the old label in the DSA display.
    front = (front + 1) % capacity;
    count--;
    return id;
}

int CircularQueue::peek() const {
    if (isEmpty()) {
        return -1;
    }
    return arr[front];
}

void CircularQueue::clear() {
    front = 0;
    rear = -1; // Rear points at the last inserted item, as in the field manual.
    count = 0;
    for (int i = 0; i < capacity; i++) {
        arr[i] = 0;
    }
}
