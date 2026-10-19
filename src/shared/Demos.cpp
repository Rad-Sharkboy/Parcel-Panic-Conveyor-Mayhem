#include "Demos.h"

// Construct the practice structures at their starting capacities.
Demos::Demos() : queue(Config::DemoCapacity), heap(2), table(7) {
    mode = 0;
    step = 0;
}

void Demos::open(int which) {
    mode = which;
    step = 0;
    queue.clear();
    heap.clear();
    table.reset(7);
    // Perform real insertions now, so opening a demo immediately shows useful data.
    for (int i = 0; i < 3; i++) {
        advance();
    }
    event = "3 sample entries inserted. Next operation continues; Restart sample begins again.";
}

void Demos::advance() {
    // These are practice objects. Operations never change the live game's structures.
    if (mode == 1) {
        queueStep();
    } else if (mode == 2) {
        heapStep();
    } else if (mode == 3) {
        hashStep();
    }
}

void Demos::queueStep() {
    int id;
    if (step < 6) {
        id = 101 + step;
        queue.enqueue(id);
        event = "enqueue #" + std::to_string(id);
    } else if (step == 6) {
        bool added = queue.enqueue(999);
        if (added) {
            event = "enqueue #999 succeeded";
        } else {
            event = "enqueue #999 rejected: FULL (queue unchanged)";
        }
    } else if (step < 9) {
        id = queue.dequeue();
        event = "dequeue #" + std::to_string(id);
    } else if (step < 11) {
        id = 107 + step - 9;
        queue.enqueue(id);
        event = "enqueue #" + std::to_string(id) + ": rear wraps to slot " + std::to_string(queue.getRear());
    } else if (step < 17) {
        id = queue.dequeue();
        event = "dequeue #" + std::to_string(id);
    } else if (step == 17) {
        id = queue.dequeue();
        if (id == -1) {
            event = "dequeue rejected: EMPTY (queue unchanged)";
        } else {
            event = "dequeue #" + std::to_string(id);
        }
    } else {
        open(1);
        return;
    }
    step++;
}

void Demos::heapStep() {
    int deadlines[6] = {90, 30, 60, 30, 10, 50};
    if (step < 6) {
        HeapEntry entry;
        entry.id = 201 + step;
        entry.deadline = deadlines[step];
        entry.sequence = step;
        heap.insert(entry);
        event = "insert #" + std::to_string(entry.id) + ": sift up; ties use arrival order";
    } else {
        HeapEntry entry;
        if (heap.extract(entry)) {
            event = "extract-min #" + std::to_string(entry.id) + " deadline " +
                    std::to_string(entry.deadline) + ": sift down";
        } else if (step == 12) {
            event = "extract-min rejected: EMPTY";
        } else {
            open(2);
            return;
        }
    }
    step++;
}

void Demos::hashStep() {
    Parcel parcel;
    if (step < 3) {
        parcel.id = 1 + 7 * step; // 1, 8 and 15 all have remainder 1 when divided by 7.
        int home = table.hash(parcel.id);
        table.insert(parcel);
        event = "insert #" + std::to_string(parcel.id) + ": home=" + std::to_string(home) +
                "; probe to a free bucket";
    } else if (step == 3) {
        if (table.find(8) != nullptr) {
            event = "find #8: SUCCESS after probing home bucket";
        } else {
            event = "find #8: missing";
        }
    } else if (step == 4) {
        if (table.find(99) != nullptr) {
            event = "find #99: success";
        } else {
            event = "find #99: MISSING, stop at EMPTY";
        }
    } else if (step == 5) {
        table.erase(8);
        event = "erase #8: TOMBSTONE keeps the search chain intact";
    } else if (step == 6) {
        if (table.find(15) != nullptr) {
            event = "find #15: SUCCESS across the tombstone";
        } else {
            event = "find #15: missing";
        }
    } else if (step == 7) {
        parcel.id = 22;
        table.insert(parcel);
        event = "insert #22: reuse the deleted bucket in the collision chain";
    } else if (step == 8) {
        parcel.id = 2;
        table.insert(parcel);
        event = "insert #2: inspect the buckets";
    } else if (step == 9) {
        parcel.id = 3;
        table.insert(parcel);
        event = "insert #3: table reaches its load limit, so rehash";
    } else if (step == 10) {
        if (table.find(22) != nullptr) {
            event = "find #22 after rehash: SUCCESS";
        } else {
            event = "find #22: missing";
        }
    } else {
        open(3);
        return;
    }
    step++;
}
