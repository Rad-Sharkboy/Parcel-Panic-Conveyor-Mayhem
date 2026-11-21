#ifndef DISPATCHSYSTEM_H
#define DISPATCHSYSTEM_H // Prevent this header being included twice in the same source file.

#include "HashTable.h"
#include "MinHeap.h"
#include "Activity.h"

class DispatchSystem {
private:
    int nextSequence = 0; // Monotonic tie-breaker assigned to each staged parcel.
    int nextLoadTick = 0; // Earliest simulation tick when the loader can dispatch next.

    // Scans active registry buckets and fails any non-terminal parcel past its deadline.
    void scanExpiredParcels(int tick);
    // Pops the highest-priority valid parcel from the MinHeap and updates score/streak.
    void dispatchNextReadyParcel(int tick, int interval);
    // Extends deadlines for active/queued parcels when an Express parcel is dispatched.
    void applyExpressDeadlineBonus();

public:
    HashTable registry;
    MinHeap ready;
    ActivityLog activity;

    int ticksUntilLoad(int tick) const { return tick < nextLoadTick ? nextLoadTick - tick : 0; }
    int score = 0, streak = 0, delivered = 0, failed = 0;
    int lastEventTick = -100, lastEventId = 0, lastPoints = 0;

    // Reset all owned structures, counters and loader timing, O(capacity).
    void reset();
    // Dock staging; only ON_CONVEYOR records accepted, no duplicate entries.
    void arrive(int id, Destination dock, int tick);
    // Terminal failure and score penalty exactly once, O(1) expected.
    void fail(int id, ParcelStatus reason, int tick);
    // Deadline scan O(registry capacity), then heap dispatch O(log n) per entry.
    void update(int tick, int interval);
    int multiplier() const;
};

#endif