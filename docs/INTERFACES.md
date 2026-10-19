# Main classes and functions

A header (`.h`) lists what a class provides. Its `.cpp` file contains the working code.

| Class | Main functions | What to remember |
|---|---|---|
| `CircularQueue` | `enqueue(id)`, `dequeue()`, `peek()` | Add at rear; remove at front. Enqueue returns false if full. Dequeue/peek return -1 if empty. |
| `CommandStack` | `push(command)`, `pop()`, `isEmpty()` | Last change is undone first. An empty pop returns a command with cell -1. |
| `MinHeap` | `insert(entry)`, `extract(output)`, `peek(output)` | Lowest deadline wins; equal deadlines use arrival order. Extract/peek return false if empty. |
| `HashTable` | `insert(parcel)`, `find(id)`, `erase(id)` | Find returns a parcel pointer, or `nullptr` if missing. Insert also updates an existing ID. |
| `ConveyorSystem` | `feed`, `move`, `switchJunction`, `undo`, `brake` | Moves IDs between input queues and belt cells. |
| `DispatchSystem` | `arrive`, `update`, `fail`, `reset` | Checks docks/deadlines, loads parcels and updates scores. |
| `Simulation` | `load(level)`, `step()`, `next(delta)` | Runs the rules and changes levels. |
| `Demos` | `open(mode)`, `advance()` | Opens a three-entry sample; then performs one operation per click/Enter. |
| `Game` | `run`, `input`, `syncParcels`, `inspectParcel` | Runs the window, controls and visual state. Some helpers are private. |
| `Renderer` / `Ui` | Drawing functions | Read game state and draw it using Raylib. |

`Parcel` holds an ID, destination, type, deadline and status. `LevelDefinition` holds a level's cells and settings. `Config.h` contains tuning values.

## Memory rules

- Each DSA class deletes its own arrays or nodes. Copies get separate storage, as taught in Lab 01.
- Queues, cells and heap entries store **IDs**. The hash table stores the parcel records.
- A hash-table resize moves records to a new array. Look up an ID again after inserting; do not keep an old parcel pointer across a resize.
- `size()` and the getters let the UI inspect a structure. Indexed getters need an index within the valid range.
- `clear()` empties a structure. `HashTable::reset(capacity)` also replaces its array.

`lastEntered[]` remembers actual queue departures for “Last to belt.” The hover card stores an ID and a short timer. Neither changes the game rules.
