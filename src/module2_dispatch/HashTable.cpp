#include "HashTable.h"

HashTable::HashTable(int size) {
    if (size < 3) {
        size = 3;
    }
    capacity = size;
    count = 0;
    deletedCount = 0;
    buckets = new Bucket[capacity];
}

HashTable::~HashTable() {
    delete[] buckets;
}

HashTable::HashTable(const HashTable& other) {
    capacity = other.capacity;
    count = other.count;
    deletedCount = other.deletedCount;
    buckets = new Bucket[capacity];
    for (int i = 0; i < capacity; i++) {
        buckets[i] = other.buckets[i];
    }
}

HashTable& HashTable::operator=(const HashTable& other) {
    if (this == &other) {
        return *this;
    }
    delete[] buckets;
    capacity = other.capacity;
    count = other.count;
    deletedCount = other.deletedCount;
    buckets = new Bucket[capacity];
    for (int i = 0; i < capacity; i++) {
        buckets[i] = other.buckets[i];
    }
    return *this;
}

int HashTable::hash(int id) const {
    if (id < 0) {
        return 0;
    }
    return id % capacity;
}

int HashTable::findIndex(int id) const {
    if (id <= 0) {
        return -1;
    }
    int index = hash(id);
    for (int checked = 0; checked < capacity; checked++) {
        if (buckets[index].state == BucketState::Empty) {
            return -1;
        }
        if (buckets[index].state == BucketState::Occupied && buckets[index].parcel.id == id) {
            return index;
        }
        index = (index + 1) % capacity; // Linear probing: try the next bucket.
    }
    return -1;
}

Parcel* HashTable::find(int id) {
    int index = findIndex(id);
    if (index == -1) {
        return nullptr;
    }
    return &buckets[index].parcel;
}

// This version lets read-only drawing code look up a parcel without changing it.
const Parcel* HashTable::find(int id) const {
    int index = findIndex(id);
    if (index == -1) {
        return nullptr;
    }
    return &buckets[index].parcel;
}

void HashTable::rehash(int newCapacity) {
    Bucket* oldBuckets = buckets;
    int oldCapacity = capacity;
    buckets = new Bucket[newCapacity];
    capacity = newCapacity;
    count = 0;
    deletedCount = 0;

    // Changing capacity changes id % capacity, so insert each live record again.
    for (int i = 0; i < oldCapacity; i++) {
        if (oldBuckets[i].state == BucketState::Occupied) {
            insert(oldBuckets[i].parcel);
        }
    }
    delete[] oldBuckets;
}

bool HashTable::insert(const Parcel& parcel) {
    if (parcel.id <= 0) {
        return false;
    }
    int existing = findIndex(parcel.id);
    if (existing != -1) {
        buckets[existing].parcel = parcel;
        return true;
    }

    // Grow before 70% of slots are used/deleted, keeping probe chains short.
    int usedSlots = count + deletedCount + 1;
    if (usedSlots * 10 >= capacity * 7) {
        rehash(capacity * 2 + 1);
    }
    int index = hash(parcel.id);
    while (buckets[index].state == BucketState::Occupied) {
        index = (index + 1) % capacity;
    }
    if (buckets[index].state == BucketState::Tombstone) {
        deletedCount--;
    }
    buckets[index].parcel = parcel;
    buckets[index].state = BucketState::Occupied;
    count++;
    return true;
}

bool HashTable::erase(int id) {
    int index = findIndex(id);
    if (index == -1) {
        return false;
    }
    // An empty slot would hide later colliding IDs; a tombstone means 'keep looking'.
    buckets[index].state = BucketState::Tombstone;
    count--;
    deletedCount++;
    return true;
}

void HashTable::clear() {
    for (int i = 0; i < capacity; i++) {
        buckets[i] = Bucket();
    }
    count = 0;
    deletedCount = 0;
}

void HashTable::reset(int size) {
    if (size < 3) {
        size = 3;
    }
    delete[] buckets;
    capacity = size;
    buckets = new Bucket[capacity];
    count = 0;
    deletedCount = 0;
}
