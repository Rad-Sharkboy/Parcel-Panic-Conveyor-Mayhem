#include "CircularQueue.h"
CircularQueue::CircularQueue(int capacity) : values_(new int[capacity > 0 ? capacity : 1]{}), capacity_(capacity > 0 ? capacity : 1) {}
CircularQueue::~CircularQueue() { delete[] values_; }
bool CircularQueue::enqueue(int id) {
    if (size_ == capacity_) return false;
    values_[rear_] = id; rear_ = (rear_ + 1) % capacity_; ++size_; return true;
}
bool CircularQueue::dequeue(int& output) {
    if (size_ == 0) return false;
    output = values_[front_]; values_[front_] = 0;
    front_ = (front_ + 1) % capacity_; --size_; return true;
}
bool CircularQueue::peek(int& output) const { if (size_ == 0) return false; output = values_[front_]; return true; }
void CircularQueue::clear() { front_ = rear_ = size_ = 0; for (int i = 0; i < capacity_; ++i) values_[i] = 0; }
