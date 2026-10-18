#ifndef DISPATCHSYSTEM_H
#define DISPATCHSYSTEM_H // Prevent this header being included twice in the same source file.
#include "HashTable.h"
#include "MinHeap.h"
class DispatchSystem {
    int nextSequence = 0;
    int nextLoadTick = 0;

public:
    HashTable registry;
    MinHeap ready;
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
