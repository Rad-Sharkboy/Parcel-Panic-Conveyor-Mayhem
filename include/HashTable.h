#pragma once
#include "Parcel.h"
enum class BucketState { Empty, Occupied, Tombstone };
struct Bucket { Parcel parcel; BucketState state = BucketState::Empty; };
class HashTable {
    Bucket* buckets_; int size_ = 0, tombstones_ = 0, capacity_;
    void rehash(int capacity);
public:
    explicit HashTable(int capacity = Config::HashCapacity);
    ~HashTable();
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    // Insert/update positive ID, expected O(1), O(n) rehash. False for invalid ID.
    bool insert(const Parcel& parcel);
    // Temporary pointer invalidated by rehash. Expected O(1), worst O(n).
    Parcel* find(int id);
    const Parcel* find(int id) const;
    // Mark tombstone; false for absent ID. Expected O(1), worst O(n).
    bool erase(int id);
    void clear();
    // Release all records and restore a chosen capacity, O(capacity).
    void reset(int capacity);
    int hash(int id) const;
    int size() const { return size_; }
    int capacity() const { return capacity_; }
    const Bucket& bucket(int i) const { return buckets_[i]; }
};
