# Class interfaces and memory ownership

This is a reference for what each class owns and what its main functions return. The simulation uses C++17 independently of Raylib. See [CODE_GUIDE.md](CODE_GUIDE.md) for includes and C++ syntax, and [COMPLEXITY.md](COMPLEXITY.md) for operation costs.

## CircularQueue

Owns a fixed-capacity dynamic `int[]` containing parcel IDs.

| Function or field | Meaning |
|---|---|
| `enqueue(id)` | Add at the rear; return `false` if full |
| `dequeue()` | Remove and return the front ID; return `-1` if empty |
| `peek()` | Read the front ID without removing it; return `-1` if empty |
| `front`, `rear` | Start at `0` and `-1`; rear means the **last inserted** slot |
| `size()`, `getCapacity()`, `getFront()`, `getRear()`, `getSlot(index)` | Let drawing code inspect the actual queue |

Removed slots contain `0`; valid gameplay IDs start at `1`. `clear()` empties the queue but keeps its allocation.

## CommandStack and ConveyorSystem

`CommandStack` owns linked `StackNode` objects. Each `SwitchCommand` stores a junction's cell and old outgoing choice. `push(command)` adds at the top; `pop()` removes the newest command, returning `cell == -1` if empty. Gameplay checks `isEmpty()` before undo. `clear()` deletes all nodes.

`ConveyorSystem` owns the grid, input queues and undo stack. Cells store parcel IDs.

- `move(HashTable&, DispatchSystem&, tick)` performs one movement pass; `feed(HashTable&, tick)` admits waiting parcels onto input belts.
- `switchJunction(index, planning)` changes a route and records its old choice only during planning.
- `undo()` restores the most recent saved choice.
- `brake(index, tick)` temporarily stops a cell. Simulation ticks clear the planning history.

## MinHeap

Owns a growing `HeapEntry[]`; each entry stores ID, deadline and arrival sequence.

- `insert(entry)` adds an entry and moves it upward until the heap order is correct.
- `extract(HeapEntry& output)` removes the root; `peek(HeapEntry& output)` reads it. Both return `false` when empty, otherwise fill the caller's `output` variable.
- Earlier deadlines win. Equal deadlines use arrival sequence, so the earlier arrival wins.
- `getEntry(index)` returns a read-only entry for drawing. `clear()` resets the count and keeps the array.

## HashTable and DispatchSystem

`HashTable` owns a `Bucket[]`. Positive parcel IDs are its keys.

| Function | Result |
|---|---|
| `insert(const Parcel&)` | Add a record, or update the record with the same ID |
| `find(id)` | Return a temporary record pointer, or `nullptr` when absent; the const version allows read-only access |
| `erase(id)` | Remove a record and leave a tombstone so later collisions remain searchable |
| `clear()` | Empty the buckets while keeping the allocation |
| `reset(capacity)` | Replace the array; minimum capacity is three |
| `getBucket(index)` | Return a read-only bucket for inspection |

`DispatchSystem` owns the hash table `registry` and heap `ready`:

- `arrive(id, dock, tick)` checks the destination and adds a correct arrival to the heap once.
- `update(tick, interval)` checks deadlines and operates the shared loader.
- A failed parcel receives one penalty. Leftover queue/cell/heap IDs are removed when next processed.
- `reset()` clears records, scores and timers while retaining the reusable array allocations.

## Memory rules shared by the four structures

Each structure has a destructor, copy constructor and copy assignment operator, following Lab 01's Rule of Three. Copies allocate independent arrays or nodes. Public indexed getters assume a valid index; callers stay within size/capacity.

The hash table owns parcel records. Queues and heap entries store only IDs. **IDs remain valid when the table grows; pointers to its old array do not.** Keep IDs between operations and look up a temporary pointer when needed. Finished records remain available for inspection until the level restarts.

## Game and presentation classes

| Class | Responsibility and main functions |
|---|---|
| `Parcel` | Record of ID, destination, kind, status, deadline and current/previous cell. `eventCell` and `pointsAwarded` save the outcome for animation. |
| `LevelDefinition` | Level settings and grid. `makeLevel(index)` creates one of four layouts; indices wrap in either direction. |
| `Simulation` | Owns conveyor and dispatch modules. `load(index)` resets a run; `step()` advances a tick; `next(delta)` changes level without awarding victory. |
| `Demos` | Owns separate practice structures. `open(mode)` resets them and performs three real insertions; `advance()` performs the next operation. Requires no Raylib. |
| `Game` | Handles input, timing and screens. `run()` opens the window, runs until quit, unloads fonts and closes the window. `syncParcels()` reads records; `effects(dt)` updates visual motion and effects. No command-line switches or diagnostic exports. |
| `Renderer` | Reads game state and draws it without changing simulation records. Menu/demo functions are in `Screens.cpp`. |
| `Ui` | Drawing helpers and button rectangles shared by input and rendering. `loadFonts()` runs after window creation; `unloadFonts()` runs before closing. |

`inspectedLane` selects the live input queue. `lastEntered[]` remembers actual input-to-belt transfers separately from waiting queue contents. `showHelp` selects Help instead of Factory info.

`Game::inspectParcel(dt)` checks the mouse against drawn parcel positions and keeps an ID plus a three-second display timer. `Renderer::parcelInfo` looks up that ID for the fixed sidebar card.

Pause and demos stop live movement and world animation; paused junction clicks/undo still turn the arrow. Demo buttons and Enter/Backspace/Esc advance, restart or close the sample. Footer buttons also open F1–F3. F4 and Shift+F4 change level immediately, including from a demo.
