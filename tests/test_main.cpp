#include "Simulation.h"
#include "Demos.h"
#include <iostream>
#include <string>
#include <type_traits>
namespace {
int checks = 0, failures = 0, cases = 0;
void check(bool condition, const char* expression, int line) {
    ++checks;
    if (!condition) { ++failures; std::cerr << "  FAIL line " << line << ": " << expression << '\n'; }
}
#define CHECK(x) check((x), #x, __LINE__)
void run(const char* name, void (*test)()) {
    ++cases; int before = failures; test();
    std::cout << (failures == before ? "PASS " : "FAIL ") << name << '\n';
}
Parcel record(int id, int deadline = 1000) { Parcel p; p.id = id; p.deadline = deadline; return p; }
void queueCases() {
    CircularQueue q(3); int n = -1;
    CHECK(!q.dequeue(n)); CHECK(n == -1); CHECK(!q.peek(n));
    CHECK(q.enqueue(7)); CHECK(q.peek(n) && n == 7); CHECK(q.dequeue(n) && n == 7);
    CHECK(q.enqueue(1)); CHECK(q.enqueue(2)); CHECK(q.enqueue(3)); CHECK(!q.enqueue(4));
    CHECK(q.size() == 3); CHECK(q.dequeue(n) && n == 1); CHECK(q.enqueue(4));
    CHECK(q.dequeue(n) && n == 2); CHECK(q.dequeue(n) && n == 3); CHECK(q.dequeue(n) && n == 4);
    for (int i = 0; i < 1000; ++i) { CHECK(q.enqueue(i)); CHECK(q.dequeue(n) && n == i); }
    q.clear(); CHECK(q.front() == 0 && q.rear() == 0 && q.size() == 0);
    CircularQueue tiny(0); CHECK(tiny.capacity() == 1);
}
void heapCases() {
    MinHeap heap(1); HeapEntry e; CHECK(!heap.extract(e)); CHECK(!heap.peek(e));
    for (int i = 999; i >= 0; --i) heap.insert({i + 1, i / 3, static_cast<unsigned long long>(i)});
    CHECK(heap.capacity() >= 1000); CHECK(heap.peek(e) && e.deadline == 0 && e.sequence == 0);
    for (int i = 0; i < 1000; ++i) { CHECK(heap.extract(e)); CHECK(e.sequence == static_cast<unsigned long long>(i)); }
    CHECK(!heap.extract(e)); heap.insert({1, 3, 0}); heap.clear(); CHECK(heap.size() == 0);
}
void hashCases() {
    HashTable table(7); CHECK(!table.find(10)); CHECK(!table.erase(10)); CHECK(!table.insert(record(0)));
    CHECK(table.insert(record(1))); CHECK(table.insert(record(8))); CHECK(table.insert(record(15)));
    CHECK(table.hash(1) == table.hash(8)); CHECK(table.bucket(2).parcel.id == 8);
    CHECK(table.erase(8)); CHECK(table.find(15)); CHECK(!table.find(8));
    Parcel changed = record(15, 33); table.insert(changed); CHECK(table.size() == 2); CHECK(table.find(15)->deadline == 33);
    table.insert(record(22)); CHECK(table.bucket(2).parcel.id == 22);
    for (int i = 100; i < 1000; ++i) table.insert(record(i));
    CHECK(table.capacity() > 7);
    for (int i = 100; i < 1000; ++i) CHECK(table.find(i) && table.find(i)->id == i);
    CHECK(table.find(15)->deadline == 33); table.clear(); CHECK(table.size() == 0 && !table.find(15));
}
void stackCases() {
    CommandStack stack; SwitchCommand c{}; CHECK(!stack.pop(c));
    stack.push({1, 0}); stack.push({2, 1}); CHECK(stack.size() == 2);
    CHECK(stack.pop(c) && c.cell == 2 && c.oldChoice == 1); CHECK(stack.pop(c) && c.cell == 1);
    stack.push({3, 0}); stack.clear(); CHECK(stack.size() == 0 && !stack.pop(c));
    ConveyorSystem conveyor; conveyor.reset(0); int j = conveyor.layout.junctions[0];
    CHECK(conveyor.switchJunction(j, true)); CHECK(conveyor.layout.cells[j].choice == 1);
    CHECK(conveyor.undo()); CHECK(conveyor.layout.cells[j].choice == 0); CHECK(!conveyor.undo());
    CHECK(!conveyor.switchJunction(-1, true));
}
void dispatchCases() {
    DispatchSystem d;
    for (int i = 1; i <= 3; ++i) { Parcel p = record(i, i == 1 ? 90 : 70); p.status = ParcelStatus::OnConveyor; d.registry.insert(p); d.arrive(i, Destination::Red, 1); }
    CHECK(d.ready.size() == 3); d.arrive(1, Destination::Red, 1); CHECK(d.ready.size() == 3);
    d.update(2, 5); CHECK(d.registry.find(2)->status == ParcelStatus::Dispatched);
    d.update(3, 5); CHECK(d.delivered == 1);
    d.update(7, 5); CHECK(d.registry.find(3)->status == ParcelStatus::Dispatched);
    d.update(12, 5); CHECK(d.registry.find(1)->status == ParcelStatus::Dispatched);
    CHECK(d.delivered == 3 && d.multiplier() == 2 && d.score == 400);
    Parcel bad = record(4); bad.status = ParcelStatus::OnConveyor; d.registry.insert(bad); d.arrive(4, Destination::Blue, 13);
    CHECK(d.registry.find(4)->status == ParcelStatus::Rejected); CHECK(d.failed == 1 && d.streak == 0);
    d.fail(4, ParcelStatus::Expired, 500); CHECK(d.failed == 1);
    Parcel notReady = record(5); notReady.status = ParcelStatus::Queued; d.registry.insert(notReady); d.arrive(5, Destination::Red, 1);
    CHECK(d.ready.size() == 0 && d.registry.find(5)->status == ParcelStatus::Queued);
}
void deadlines() {
    DispatchSystem d; Parcel p = record(1, 2); p.status = ParcelStatus::Queued; d.registry.insert(p);
    d.update(2, 1); CHECK(d.failed == 0); d.update(3, 1); CHECK(d.failed == 1);
    d.update(4, 1); CHECK(d.failed == 1);
    d.reset(); p = record(1, 2); p.status = ParcelStatus::OnConveyor; d.registry.insert(p); d.arrive(1, Destination::Red, 1);
    d.update(3, 1); CHECK(d.failed == 1 && d.delivered == 0 && d.ready.size() == 0);
    d.reset(); p = record(1); p.status = ParcelStatus::OnConveyor; p.kind = ParcelKind::Express; d.registry.insert(p);
    Parcel waiting = record(2, 100); waiting.status = ParcelStatus::Queued; d.registry.insert(waiting);
    d.arrive(1, Destination::Red, 1); d.update(1, 1); CHECK(d.registry.find(2)->deadline == 100 + Config::ExpressBonusTicks);
}
bool reaches(const LevelDefinition& l, int start, Destination destination, bool fragile, int depth = 0) {
    if (start < 0 || start >= Config::Cells || depth >= Config::Cells) return false;
    const Cell& c = l.cells[start];
    if (c.kind == CellKind::Empty || (fragile && c.kind == CellKind::Hazard)) return false;
    if (c.kind == CellKind::Dock) return c.dock == destination;
    return reaches(l, c.next[0], destination, fragile, depth + 1) || (c.kind == CellKind::Junction && reaches(l, c.next[1], destination, fragile, depth + 1));
}
void layouts() {
    for (int level = 0; level < 4; ++level) {
        LevelDefinition l = makeLevel(level); CHECK(l.inputCount >= 2); CHECK(l.junctionCount >= 2);
        for (int input = 0; input < l.inputCount; ++input) {
            int destinations = l.tuning.specials ? 4 : l.tuning.colors;
            for (int d = 0; d < destinations; ++d) { CHECK(reaches(l, l.inputs[input], static_cast<Destination>(d), false)); CHECK(reaches(l, l.inputs[input], static_cast<Destination>(d), true)); }
        }
        for (int c = 0; c < Config::Cells; ++c) {
            const Cell& cell = l.cells[c];
            if (cell.kind == CellKind::Empty || cell.kind == CellKind::Dock) continue;
            CHECK(cell.next[0] >= 0 && cell.next[0] < Config::Cells);
            if (cell.kind == CellKind::Junction) CHECK(cell.next[1] >= 0 && cell.next[1] < Config::Cells);
        }
    }
}
void generation() {
    Simulation s; s.load(2); CHECK(s.spawn()); CHECK(s.spawn()); CHECK(s.spawn());
    CHECK(s.generated == 3 && s.dispatch.registry.size() == 3);
    CHECK(s.dispatch.registry.find(1)->status == ParcelStatus::Queued);
    CHECK(s.dispatch.registry.find(2)->kind == ParcelKind::Express);
    CHECK(s.dispatch.registry.find(2)->deadline == Config::ExpressDeadlineTicks);
    CHECK(s.dispatch.registry.find(3)->kind == ParcelKind::Fragile);
    for (int i = 0; i < 40; ++i) s.spawn();
    CHECK(s.generated == 16);
    CHECK(!s.spawn()); CHECK(s.dispatch.registry.size() == 16);
}
void movement() {
    Simulation s; s.load(0); CHECK(s.spawn()); s.conveyor.feed(s.dispatch.registry, 0);
    int start = s.conveyor.layout.inputs[0]; int target = s.conveyor.layout.cells[start].next[0];
    CHECK(s.conveyor.brake(start, 0)); CHECK(!s.conveyor.brake(start, 1));
    s.conveyor.move(s.dispatch.registry, s.dispatch, 1); CHECK(s.conveyor.layout.cells[start].parcel == 1);
    s.conveyor.move(s.dispatch.registry, s.dispatch, Config::BrakeTicks); CHECK(s.conveyor.layout.cells[target].parcel == 1);
    CHECK(s.dispatch.registry.find(1)->cell == target); CHECK(s.conveyor.layout.cells[start].parcel == 0);
    // A second parcel immediately behind the first must wait for the occupied snapshot cell.
    Parcel p = record(2); p.status = ParcelStatus::OnConveyor; p.cell = start; s.dispatch.registry.insert(p); s.conveyor.layout.cells[start].parcel = 2; s.generated = 2;
    s.conveyor.move(s.dispatch.registry, s.dispatch, 15);
    CHECK(s.conveyor.layout.cells[start].parcel == 2); CHECK(s.conveyor.blocked >= 1); CHECK(s.validate());
}
void hazards() {
    Simulation s; s.load(2); int j = cellAt(6, 5); int hazard = cellAt(7, 5);
    Parcel p = record(1); p.kind = ParcelKind::Fragile; p.status = ParcelStatus::OnConveyor; p.cell = j;
    s.dispatch.registry.insert(p); s.conveyor.layout.cells[j].parcel = 1;
    s.conveyor.move(s.dispatch.registry, s.dispatch, 2);
    CHECK(s.dispatch.registry.find(1)->status == ParcelStatus::Broken); CHECK(s.conveyor.layout.cells[hazard].parcel == 0 && s.dispatch.failed == 1);
}
void resetSkip() {
    Simulation s; s.load(0); for (int i = 0; i < 50; ++i) s.step();
    s.conveyor.switchJunction(s.conveyor.layout.junctions[0], true); s.next();
    CHECK(s.level == 1 && s.tick == 0 && s.generated == 0); CHECK(s.dispatch.registry.size() == 0 && s.dispatch.ready.size() == 0 && s.conveyor.history.size() == 0);
    CHECK(s.dispatch.score == 0 && s.jam == 0 && s.outcome == Outcome::Playing);
    for (int i = 0; i < Config::Cells; ++i) CHECK(s.conveyor.layout.cells[i].parcel == 0);
    s.next(-1); CHECK(s.level == 0); s.next(-1); CHECK(s.level == 3); s.next(); CHECK(s.level == 0);
    s.load(2); s.spawn(); s.load(2); CHECK(s.dispatch.registry.size() == 0 && s.tick == 0);
}
void autopilot(Simulation& s) {
    for (int i = 0; i < s.conveyor.layout.junctionCount; ++i) {
        int index = s.conveyor.layout.junctions[i]; Cell& c = s.conveyor.layout.cells[index]; const Parcel* p = s.dispatch.registry.find(c.parcel);
        if (!p || p->status != ParcelStatus::OnConveyor) continue;
        bool first = reaches(s.conveyor.layout, c.next[0], p->destination, p->kind == ParcelKind::Fragile);
        bool second = reaches(s.conveyor.layout, c.next[1], p->destination, p->kind == ParcelKind::Fragile);
        if (first || second) c.choice = first ? 0 : 1;
    }
}
void playthroughs() {
    for (int level = 0; level < 4; ++level) {
        Simulation s; s.load(level);
        while (s.outcome == Outcome::Playing && s.tick <= Config::MaxLevelTicks) { autopilot(s); s.step(); CHECK(s.validate()); }
        std::cout << "  level " << level + 1 << ": delivered=" << s.dispatch.delivered << " failed=" << s.dispatch.failed << " score=" << s.dispatch.score << " ticks=" << s.tick << '\n';
        CHECK(s.outcome == Outcome::Won); CHECK(s.generated == s.conveyor.layout.tuning.total);
        for (int i = 0; i < s.dispatch.registry.capacity(); ++i) {
            const Bucket& b = s.dispatch.registry.bucket(i);
            if (b.state == BucketState::Occupied && b.parcel.status != ParcelStatus::Dispatched) CHECK(b.parcel.status == ParcelStatus::Expired);
        }
        CHECK(s.dispatch.delivered + s.dispatch.failed == s.generated);
        int tick = s.tick; s.step(); CHECK(s.tick == tick);
    }
}
void idleTermination() {
    for (int level = 0; level < 4; ++level) {
        Simulation s; s.load(level); while (s.outcome == Outcome::Playing) s.step();
        CHECK(s.tick <= Config::MaxLevelTicks); CHECK(s.outcome == Outcome::Lost);
    }
}
void jamFailure() {
    Simulation s; s.load(3);
    // Six blocked conveyors establish persistent congestion; long deadlines isolate jam failure.
    for (int i = 0; i < 6; ++i) {
        int index = cellAt(i, 1); Parcel p = record(i + 1, 10000); p.status = ParcelStatus::OnConveyor; p.cell = index;
        s.dispatch.registry.insert(p); s.conveyor.layout.cells[index].parcel = p.id; s.conveyor.layout.cells[index].next[0] = -1;
    }
    s.generated = 6;
    while (s.outcome == Outcome::Playing && s.tick < 200) s.step();
    CHECK(s.outcome == Outcome::Lost && s.jam == Config::JamLimit);
}
void demoSequences() {
    Demos d;
    for (int repeat = 0; repeat < 2; ++repeat) {
        d.open(1);
        for (int i = 0; i < 6; ++i) d.advance();
        CHECK(d.queue.size() == 6); d.advance(); CHECK(d.queue.size() == 6);
        for (int i = 0; i < 4; ++i) d.advance();
        CHECK(d.queue.size() == 6 && d.queue.front() == 2 && d.queue.rear() == 2);
        for (int i = 0; i < 7; ++i) d.advance();
        CHECK(d.queue.size() == 0);
        d.open(2); for (int i = 0; i < 6; ++i) d.advance();
        HeapEntry root; CHECK(d.heap.peek(root) && root.id == 205);
        d.advance(); CHECK(d.heap.peek(root) && root.id == 202);
        d.advance(); CHECK(d.heap.peek(root) && root.id == 204);
        d.open(3); CHECK(d.table.capacity() == 7);
        for (int i = 0; i < 3; ++i) d.advance();
        CHECK(d.table.bucket(1).parcel.id == 1 && d.table.bucket(2).parcel.id == 8 && d.table.bucket(3).parcel.id == 15);
        for (int i = 0; i < 3; ++i) d.advance();
        CHECK(d.table.bucket(2).state == BucketState::Tombstone && d.table.find(15));
        for (int i = 0; i < 4; ++i) d.advance();
        CHECK(d.table.capacity() == 15 && d.table.find(22));
    }
}
void rushAndHistory() {
    Simulation s; s.load(1); s.tick = Config::RushPeriod - Config::RushDuration - 1; CHECK(s.rushWarning());
    ++s.tick; CHECK(s.rush()); s.tick = Config::RushPeriod; CHECK(!s.rush());
    int j = s.conveyor.layout.junctions[0]; s.conveyor.switchJunction(j, true); CHECK(s.conveyor.history.size() == 1);
    s.step(); CHECK(s.conveyor.history.size() == 0);
}
}
int main() {
    static_assert(!std::is_copy_constructible<CircularQueue>::value, "Queue must not shallow-copy");
    static_assert(!std::is_copy_constructible<HashTable>::value, "Registry must not shallow-copy");
    run("circular queue: empty/full/overflow/wraparound/repetition", queueCases);
    run("min heap: empty/stable ties/1000 entries/resizing", heapCases);
    run("hash table: collisions/update/tombstones/rehash", hashCases);
    run("linked stack: LIFO/clear/junction undo", stackCases);
    run("dispatch: staging/order/capacity/scoring/wrong dock", dispatchCases);
    run("deadline expiry once and express time bonus", deadlines);
    run("four layouts: destination reachability and fragile bypasses", layouts);
    run("generation: unique IDs/types/queue overflow", generation);
    run("movement: brakes/occupancy/no double move", movement);
    run("fragile crusher terminal event", hazards);
    run("restart and F4 level navigation semantics", resetSkip);
    run("four deterministic winning full playthroughs", playthroughs);
    run("uncontrolled runs terminate", idleTermination);
    run("persistent congestion triggers jam failure", jamFailure);
    run("F1-F3 repeatable real demonstration sequences", demoSequences);
    run("rush schedule and planning history boundary", rushAndHistory);
    std::cout << cases << " cases, " << checks << " checks, " << failures << " failures\n";
    return failures ? 1 : 0;
}
