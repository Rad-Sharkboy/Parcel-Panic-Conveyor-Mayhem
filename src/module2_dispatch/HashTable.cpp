#include "HashTable.h"
HashTable::HashTable(int capacity) : buckets_(new Bucket[capacity < 3 ? 3 : capacity]), capacity_(capacity < 3 ? 3 : capacity) {}
HashTable::~HashTable() { delete[] buckets_; }
int HashTable::hash(int id) const { return id < 0 ? 0 : id % capacity_; }
const Parcel* HashTable::find(int id) const {
    if (id <= 0) return nullptr;
    int start = hash(id);
    for (int p = 0; p < capacity_; ++p) {
        const Bucket& b = buckets_[(start + p) % capacity_];
        if (b.state == BucketState::Empty) return nullptr;
        if (b.state == BucketState::Occupied && b.parcel.id == id) return &b.parcel;
    }
    return nullptr;
}
Parcel* HashTable::find(int id) { return const_cast<Parcel*>(static_cast<const HashTable&>(*this).find(id)); }
void HashTable::rehash(int capacity) {
    Bucket* old = buckets_; int oldCapacity = capacity_;
    buckets_ = new Bucket[capacity]; capacity_ = capacity; size_ = tombstones_ = 0;
    for (int i = 0; i < oldCapacity; ++i) if (old[i].state == BucketState::Occupied) insert(old[i].parcel);
    delete[] old;
}
bool HashTable::insert(const Parcel& parcel) {
    if (parcel.id <= 0) return false;
    Parcel* existing = find(parcel.id);
    if (existing) { *existing = parcel; return true; }
    if ((size_ + tombstones_ + 1) * 10 >= capacity_ * 7) rehash(capacity_ * 2 + 1);
    int index = hash(parcel.id);
    while (buckets_[index].state == BucketState::Occupied) index = (index + 1) % capacity_;
    if (buckets_[index].state == BucketState::Tombstone) --tombstones_;
    buckets_[index].parcel = parcel; buckets_[index].state = BucketState::Occupied; ++size_; return true;
}
bool HashTable::erase(int id) {
    if (!find(id)) return false;
    int index = hash(id);
    while (!(buckets_[index].state == BucketState::Occupied && buckets_[index].parcel.id == id)) index = (index + 1) % capacity_;
    buckets_[index].state = BucketState::Tombstone; --size_; ++tombstones_; return true;
}
void HashTable::clear() { for (int i = 0; i < capacity_; ++i) buckets_[i] = Bucket{}; size_ = tombstones_ = 0; }

void HashTable::reset(int capacity) {
    int count = capacity < 3 ? 3 : capacity; Bucket* fresh = new Bucket[count];
    delete[] buckets_; buckets_ = fresh; capacity_ = count; size_ = tombstones_ = 0;
}
