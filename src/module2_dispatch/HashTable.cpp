#include "HashTable.h"

HashTable::HashTable(int initialCapacity) {
    if (initialCapacity < 3) {
        initialCapacity = 3;
    }
    capacity = initialCapacity;
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

// Open addressing with linear probing: walks past Tombstone buckets until Empty or matching ID.
int HashTable::findIndex(int id) const {
    if (id <= 0) {
        return -1;
    }
    int probeIndex = hash(id);
    for (int probeSteps = 0; probeSteps < capacity; probeSteps++) {
        if (buckets[probeIndex].state == BucketState::Empty) {
            return -1;
        }
        if (buckets[probeIndex].state == BucketState::Occupied && buckets[probeIndex].parcel.id == id) {
            return probeIndex;
        }
        probeIndex = (probeIndex + 1) % capacity;
    }
    return -1;
}

Parcel* HashTable::find(int id) {
    int bucketIndex = findIndex(id);
    if (bucketIndex == -1) {
        return nullptr;
    }
    return &buckets[bucketIndex].parcel;
}

// Read-only lookup overload used by rendering and inspector views.
const Parcel* HashTable::find(int id) const {
    int bucketIndex = findIndex(id);
    if (bucketIndex == -1) {
        return nullptr;
    }
    return &buckets[bucketIndex].parcel;
}

void HashTable::rehash(int newCapacity) {
    if (newCapacity < 3) {
        newCapacity = 3;
    }
    Bucket* oldBuckets = buckets;
    int oldCapacity = capacity;
    buckets = new Bucket[newCapacity];
    capacity = newCapacity;
    count = 0;
    deletedCount = 0;

    // Re-insert only live occupied records so tombstones are cleared out.
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
    int existingIndex = findIndex(parcel.id);
    if (existingIndex != -1) {
        buckets[existingIndex].parcel = parcel;
        return true;
    }

    // Rehash when active + tombstone entries reach 70% load factor to keep probe chains short.
    int occupiedSlots = count + deletedCount + 1;
    if (occupiedSlots * 10 >= capacity * 7) {
        rehash(capacity * 2 + 1);
    }
    int insertIndex = hash(parcel.id);
    while (buckets[insertIndex].state == BucketState::Occupied) {
        insertIndex = (insertIndex + 1) % capacity;
    }
    if (buckets[insertIndex].state == BucketState::Tombstone) {
        deletedCount--;
    }
    buckets[insertIndex].parcel = parcel;
    buckets[insertIndex].state = BucketState::Occupied;
    count++;
    return true;
}

bool HashTable::erase(int id) {
    int targetIndex = findIndex(id);
    if (targetIndex == -1) {
        return false;
    }
    // Mark as Tombstone instead of Empty so subsequent probe sequences remain intact.
    buckets[targetIndex].state = BucketState::Tombstone;
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

void HashTable::reset(int newSize) {
    if (newSize < 3) {
        newSize = 3;
    }
    delete[] buckets;
    capacity = newSize;
    buckets = new Bucket[capacity];
    count = 0;
    deletedCount = 0;
}