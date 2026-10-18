# Interfaces and ownership contract

Defined before implementation. The simulation uses C++17 and has no Raylib dependency.

## Module 1 — Conveyor and Routing (planned Member 1)

* `CircularQueue(int capacity)`: owns a fixed-capacity dynamic `int[]`. `enqueue(int)` and `dequeue(int&)` return success in O(1); failed operations leave state unchanged. Front/rear/count/capacity and physical slots are observable. Rear is the next insertion slot. Copying is disabled.
* `CommandStack`: owns singly linked command nodes. `push(SwitchCommand)`, `pop(SwitchCommand&)`, and `clear()` implement planning undo. Push/pop O(1); clear O(n). Copying is disabled.
* `ConveyorSystem`: owns a fixed grid of cells and four input queues. Cells contain IDs, never registry pointers. `move(HashTable&, DispatchSystem&, tick)` performs one synchronous movement pass with deterministic arbitration. `switchJunction(index, planning)` records commands only during planning; `undo()` restores a junction; `brake(index,tick)` freezes a cell temporarily. Simulation ticks clear planning history.

## Module 2 — Parcel Tracking and Dispatch (planned Member 2)

* `MinHeap`: owns a growing dynamic `HeapEntry[]` containing ID/deadline/sequence. `insert`, `extract`, `peek`, `clear`; stable deadline then sequence ordering. Insert/extract O(log n), peek O(1); growth O(n). Copying disabled.
* `HashTable`: owns an open-addressed `Bucket[]`, keyed by positive integer parcel ID. `insert(const Parcel&)` inserts or updates, `find(id)` returns a temporary pointer or null, `erase(id)` creates a tombstone, `clear()` resets records. Rehash invalidates all returned pointers. No caller retains a pointer across insertion/rehashing. Copying disabled.
* `DispatchSystem`: owns the authoritative `HashTable registry` and `MinHeap ready`. `arrive(id,dock,tick)` checks destination and stages exactly once; `update(tick, interval)` expires active parcels and uses the shared loader. Deadline failure is terminal and penalized exactly once. Conveyor/queue stale IDs are removed on their next step. `reset()` clears registry, heap, scores and timers.

## Module 3 — Shared Game and Presentation (joint)

* `Parcel`: value record with unique ID, destination, kind, lifecycle status, absolute deadline, current/previous grid cell and movement tick.
* `LevelDefinition`: value record of tuning and a handcrafted grid. `makeLevel(index)` constructs one of four layouts; index wraps modulo four.
* `Simulation`: owns the conveyor and dispatch modules and generation counters. `load(index)` resets all state and time, `step()` advances exactly one logical tick, `next(delta)` wraps levels without awarding a victory. `validate()` checks occupancy, identity and active-state consistency. No copy operations.
* `Game`: handles Raylib input, fixed-step timing, menus and isolated DSA demos. `Renderer` only reads game state to draw the factory, interpolation, feedback, dashboard and screens.

IDs survive registry rehashing; raw parcel pointers do not. Queues own no parcels. Dock arrival transitions through AT_DOCK to READY; only ready records are dispatched. Terminal records remain available until restart for inspection. Completed movement cannot be undone. No rendering function mutates the simulation.

## Inspection and validation helpers

`HashTable::reset(int capacity)` releases records and restores a minimum capacity of three; the F3 demo uses it to return to seven buckets after a rehash. `Demos::open(int mode)` resets isolated actual instances; `advance()` performs one real operation and records its result. Demos link into `parcel_core` and are tested without Raylib.

`Simulation::validate() const` checks registry size versus generation, active membership exactly once across queues/cells/heap, matching locations/statuses, and actual terminal counts versus scoring counters. Terminal stale references are allowed until cleanup. It returns a boolean and mutates no state.

`Game::run(int smokeFrames, const char* screenshot, const char* stateFile)` owns the Raylib window and returns success after cleanup. Its optional observation file is available for optional automated input verification; it reports actual screen, level, tick, pause, demo sizes/step, undo depth, brake readiness and first-junction choice. Normal play does not create this file. A smoke screenshot export failure returns a nonzero application exit code.
