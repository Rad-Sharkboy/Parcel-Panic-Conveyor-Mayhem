#include "Demos.h"
void Demos::open(int which) {
    mode = which; step = 0; queue.clear(); heap.clear(); table.reset(7);
    event = "Isolated real instances. Press ENTER to perform one operation.";
}
void Demos::advance() {
    if (mode == 1) {
        int id = 0;
        if (step < 6) { queue.enqueue(101 + step); event = "enqueue #" + std::to_string(101 + step); }
        else if (step == 6) event = queue.enqueue(999) ? "enqueue succeeded" : "enqueue #999 rejected: FULL (state unchanged)";
        else if (step < 9) { id = queue.dequeue(); event = "dequeue #" + std::to_string(id); }
        else if (step < 11) { queue.enqueue(107 + step - 9); event = "enqueue wraps rear to slot " + std::to_string(queue.getRear()); }
        else if (step < 17) { id = queue.dequeue(); event = "dequeue #" + std::to_string(id); }
        else if (step == 17) event = queue.dequeue() != -1 ? "dequeue succeeded" : "dequeue rejected: EMPTY (state unchanged)";
        else { open(1); return; }
    } else if (mode == 2) {
        const int deadlines[6] = {90, 30, 60, 30, 10, 50};
        if (step < 6) {
            heap.insert({201 + step, deadlines[step], static_cast<unsigned long long>(step)});
            event = "insert #" + std::to_string(201 + step) + ": sift up; ties use sequence";
        } else {
            HeapEntry e;
            if (heap.extract(e)) event = "extract-min #" + std::to_string(e.id) + " deadline " + std::to_string(e.deadline) + ": sift down";
            else if (step == 12) event = "extract-min rejected: EMPTY";
            else { open(2); return; }
        }
    } else if (mode == 3) {
        Parcel p;
        if (step < 3) {
            p.id = 1 + 7 * step; int home = table.hash(p.id); table.insert(p);
            event = "insert #" + std::to_string(p.id) + ": home=" + std::to_string(home) + "; linear probe to free bucket";
        } else if (step == 3) event = table.find(8) ? "find #8: SUCCESS after probing home bucket" : "find #8: missing";
        else if (step == 4) event = table.find(99) ? "find #99: success" : "find #99: MISSING, stop at EMPTY";
        else if (step == 5) { table.erase(8); event = "erase #8: TOMBSTONE preserves the probe chain"; }
        else if (step == 6) event = table.find(15) ? "find #15: SUCCESS across the tombstone" : "find #15: missing";
        else if (step == 7) { p.id = 22; table.insert(p); event = "insert #22: reuse tombstone in collision chain"; }
        else if (step == 8) { p.id = 2; table.insert(p); event = "insert #2: inspect buckets and load"; }
        else if (step == 9) { p.id = 3; table.insert(p); event = "insert #3: load threshold triggers rehash"; }
        else if (step == 10) event = table.find(22) ? "find #22 after rehash: SUCCESS" : "find #22: missing";
        else { open(3); return; }
    }
    ++step;
}
