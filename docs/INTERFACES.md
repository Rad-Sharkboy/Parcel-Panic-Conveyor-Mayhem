# Interfaces and ownership

The simulation uses C++17 and has no Raylib dependency. The [code guide](CODE_GUIDE.md) explains the syntax and lists every direct include.

## Conveyor and routing

- `CircularQueue(int capacity)`: owns a fixed-capacity dynamic `int[]`. `enqueue(int)` returns false when full. `dequeue()` returns the removed ID; `peek()` reads it without removal. Both return -1 when empty. Front starts at 0; rear starts at -1 and identifies the last inserted slot. `size()`, `getCapacity()`, `getFront()`, `getRear()` and `getSlot(index)` let the UI read the actual array. Removed slots display as zero; game IDs start at 1.
- `CommandStack`: owns singly linked `StackNode` objects. `push(SwitchCommand)` saves a junction's old choice. `pop()` returns the newest command, with `cell == -1` if empty. `isEmpty()` is checked before gameplay undo. `clear()` deletes the nodes.
- `ConveyorSystem`: owns the grid, input queues and undo stack. Cells contain IDs. `move(HashTable&, DispatchSystem&, tick)` performs one movement pass. `switchJunction(index, planning)` records changes only during planning; `undo()` restores one change; `brake(index, tick)` freezes a cell temporarily. Simulation ticks clear planning history.

## Parcel tracking and dispatch

- `MinHeap`: owns a growing `HeapEntry[]` of ID/deadline/sequence records. `insert(entry)` moves an entry upward; `extract(HeapEntry& output)` removes the root; `peek(HeapEntry& output)` reads it. The last two return false when empty and otherwise fill the caller's `output` variable. Earlier deadlines win; equal deadlines use arrival sequence. `getEntry(index)` exposes a read-only array entry for drawing.
- `HashTable`: owns a `Bucket[]` with positive parcel IDs as keys. `insert(const Parcel&)` inserts or updates. `find(id)` returns a temporary pointer, or `nullptr` if absent. Its const overload permits read-only access. `erase(id)` leaves a tombstone; `clear()` empties the retained allocation; `reset(capacity)` replaces it (minimum three buckets). `getBucket(index)` exposes a read-only bucket.
- `DispatchSystem`: owns `HashTable registry` and `MinHeap ready`. `arrive(id, dock, tick)` checks the destination and stages a parcel once. `update(tick, interval)` checks deadlines and uses the shared loader. A failed parcel receives one penalty; stale queue/cell/heap IDs are removed on subsequent processing. `reset()` clears records, scores and timers, retaining reusable allocations.

The queue, stack, heap and table each implement a destructor, copy constructor and copy assignment operator. Copies have independent storage, following Lab 01's Rule of Three. Public indexed getters assume a valid index; their callers loop within size/capacity. Complexity is documented in [COMPLEXITY.md](COMPLEXITY.md).

IDs survive registry rehashing; raw parcel pointers do not. Callers keep IDs between operations and use pointers temporarily. Queues and heap entries own no parcels. Terminal records stay in the registry until restart so the UI can inspect them.

## Game and presentation

- `Parcel`: value record with ID, destination, kind, status, deadline and current/previous cell. `eventCell` and `pointsAwarded` preserve the outcome location and reward for animation.
- `LevelDefinition`: tuning and a handcrafted grid. `makeLevel(index)` constructs one of four layouts; level indices wrap in both directions.
- `Simulation`: owns conveyor and dispatch modules. `load(index)` resets a run; `step()` advances one logical tick; `next(delta)` changes level without awarding victory.
- `Demos`: separate queue/heap/table objects. `open(mode)` clears practice data and performs three actual insertions; `advance()` continues one of three short operation sequences. It belongs to the core and does not require Raylib.
- `Game`: handles input, frame timing and screen state. `run()` creates the Raylib window, runs until quit, unloads fonts and closes the window. There are no command-line switches or diagnostic file exports. `syncParcels()` reads the registry; `effects(dt)` moves visual positions and effects. `inspectedLane` selects the live input queue displayed in the sidebar.
- `Renderer`: reads the game state and draws it; screen functions live in `Screens.cpp`. It does not change simulation records.
- `Ui`: drawing helpers and button rectangles shared with input. `loadFonts()` runs after window creation; `unloadFonts()` runs before closing it.

`Game::inspectParcel(dt)` uses visual positions for hover hit testing and keeps an ID plus a short display timer. `Renderer::parcelInfo` reads that record into a fixed sidebar card. `lastEntered[]` remembers real input-to-belt transfers; it is separate from current queue contents. `showHelp` selects Help instead of Factory info.

Pause and practice demos stop the live simulation and world animation. A paused junction can still turn in response to a click or undo. Next operation/Enter, Restart sample/Backspace and Close/Esc operate the demos. The footer buttons also open F1–F3. F4 and Shift+F4 start another level immediately, including from a demo.
