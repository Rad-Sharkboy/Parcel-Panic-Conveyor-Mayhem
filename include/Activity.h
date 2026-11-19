#ifndef ACTIVITY_H
#define ACTIVITY_H

// A small display journal, not a replacement for any of the four DSA classes.
// Record only completed operations at their real gameplay call sites.
enum class Operation { Enqueue, Dequeue, SkipQueued, Push, Pop, ClearUndo,
                       HeapInsert, Dispatch, SkipReady, Register, Rehash };
struct Activity {
    Operation operation = Operation::Enqueue;
    int id = 0;
    int detail = 0; // Lane, junction choice, deadline or previous capacity.
};
class ActivityLog {
public:
    static const int Capacity = 10;
    Activity entries[Capacity]{};
    int count = 0;
    void clear() { count = 0; }
    void add(Operation operation, int id, int detail = 0) {
        // Newest entry first; shifting at most nine records is bounded work.
        if (count < Capacity) { count++; }
        for (int i = count - 1; i > 0; i--) { entries[i] = entries[i - 1]; }
        entries[0] = {operation, id, detail};
    }
};
#endif
