#ifndef HASHTABLE_H
#define HASHTABLE_H // Prevent duplicate declarations when headers include each other.

#include "Parcel.h"

// Named states: a removed bucket must not end a search through a collision chain.
enum class BucketState { Empty, Occupied, Tombstone };
struct Bucket {
    Parcel parcel;
    BucketState state = BucketState::Empty;
};

class HashTable {
private:
    Bucket* buckets;
    int count;
    int deletedCount;
    int capacity;
    int findIndex(int id) const;
    void rehash(int newCapacity);

public:
    HashTable(int capacity = Config::HashCapacity);
    ~HashTable();
    HashTable(const HashTable& other);
    HashTable& operator=(const HashTable& other);

    // Insert/update a positive ID; false for invalid IDs. Expected O(1), with rehash work if full.
    bool insert(const Parcel& parcel);
    // Return a record pointer, or nullptr if absent. Expected O(1), worst O(n).
    // Inserting can resize the array: keep IDs, not these pointers, between operations.
    Parcel* find(int id);
    const Parcel* find(int id) const;
    // Mark the matching bucket as deleted. False when absent. Expected O(1), worst O(n).
    bool erase(int id);
    void clear();
    void reset(int capacity);
    int hash(int id) const;
    int size() const { return count; }
    int getCapacity() const { return capacity; }
    const Bucket& getBucket(int index) const { return buckets[index]; }
};

#endif
