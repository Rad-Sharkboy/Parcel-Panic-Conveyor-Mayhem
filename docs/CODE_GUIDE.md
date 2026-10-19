# Code guide for class

Use this guide to explain the project's own C++ files. It keeps the full include list and the less familiar syntax in one place. Raylib is an external graphics library downloaded by CMake; its internal code is separate from our custom data structures.

## How the class material was used

| Material | Pattern used here |
|---|---|
| Lab 01 | Dynamic memory, destructors, deep-copy constructors and assignment operators (Rule of Three) |
| Lab 02 | Ordinary arrays, `new[]`, `delete[]` and copying entries when an array grows |
| Lab 03 | Explicit temporary variables when swapping entries |
| Lab 04 and Lab 05 | Linked nodes, pointer traversal and a stack that pushes/pops at its head |
| DSA field manual | Circular-array queue with `arr`, `front`, `rear`, `capacity`, `count`, `isEmpty()` and `isFull()` |
| PC-Builder-Tycoon reference | Separate headers/source files, `#ifndef` guards, ordinary loops and short explanatory comments |

The recursion/backtracking material was reviewed, but these operations can be written as loops without adding recursion. The heap and hash table still need study when those topics are taught. Their exact classroom implementation is not yet known.

The queue initializes `front = 0`, `rear = -1`, `count = 0`. Enqueue advances rear before storing an ID. Rear therefore means **last inserted**, not next write. Dequeue returns the ID, or -1 if empty. Cleared display slots contain 0; gameplay IDs start at 1.

The stack uses a `StackNode` containing a command and `next`, with `top` pointing to its first node. A command has two fields instead of the lab's single integer: which junction changed and its old choice. Push links a new node before top; pop removes and deletes top. Copying appends nodes using a tail pointer so the copied stack keeps the same order.

No prebuilt stack, queue, priority queue, vector, list or map implements these structures. The code uses no templates, lambdas, smart pointers, move operations or exception-handling framework. `<algorithm>`, `<utility>`, `#pragma once`, cast-based hash lookup and the command-line diagnostic parser have been removed. Member names no longer have trailing underscores.

## Which library headers remain, and why

| Include | Purpose | Where it is directly included |
|---|---|---|
| `<string>` | `std::string` stores messages and font paths; `std::to_string` turns an ID into text; `.c_str()` passes text to Raylib | `Demos.h`, `Ui.cpp` |
| `<cmath>` | `sin`, `cos`, `atan2`, `sqrt`, `fmod` and `round` supply angles, distances, repeating motion and pixel rounding | `Game.cpp`, `Renderer.cpp`, `Screens.cpp`, `Ui.cpp` |
| `"raylib.h"` | Window, keyboard/mouse, shapes, timing, fonts and text functions | `Game.h`, `Ui.h` |

There is no direct `<iostream>` include because the game has no console input/output. There is no `<climits>` include because it does not use integer-limit macros. Keeping standard math functions avoids writing harder formulas ourselves. Each standard-library include has a purpose comment beside it.

Quoted project headers declare **our own** classes, records or settings. They do not provide a ready-made data structure. Including `Game.h`, for example, brings in the game's other headers indirectly. The table below records every direct include in every project `.h` and `.cpp`, excluding downloaded/build files.

## Complete direct-include inventory

| File | Direct includes |
|---|---|
| [include/CircularQueue.h](../include/CircularQueue.h) | `"Config.h"` |
| [include/CommandStack.h](../include/CommandStack.h) | None |
| [include/Config.h](../include/Config.h) | None |
| [include/ConveyorSystem.h](../include/ConveyorSystem.h) | `"Level.h"`, `"CircularQueue.h"`, `"CommandStack.h"`, `"DispatchSystem.h"` |
| [include/Demos.h](../include/Demos.h) | `"CircularQueue.h"`, `"MinHeap.h"`, `"HashTable.h"`, `<string>` |
| [include/DispatchSystem.h](../include/DispatchSystem.h) | `"HashTable.h"`, `"MinHeap.h"` |
| [include/Game.h](../include/Game.h) | `"Simulation.h"`, `"Demos.h"`, `"raylib.h"` |
| [include/HashTable.h](../include/HashTable.h) | `"Parcel.h"` |
| [include/Level.h](../include/Level.h) | `"Config.h"`, `"Parcel.h"` |
| [include/MinHeap.h](../include/MinHeap.h) | `"Config.h"` |
| [include/Parcel.h](../include/Parcel.h) | `"Config.h"` |
| [include/Renderer.h](../include/Renderer.h) | `"Game.h"` |
| [include/Simulation.h](../include/Simulation.h) | `"ConveyorSystem.h"` |
| [include/Ui.h](../include/Ui.h) | `"raylib.h"`, `"Parcel.h"` |
| [src/main.cpp](../src/main.cpp) | `"Game.h"` |
| [src/module1_conveyor/CircularQueue.cpp](../src/module1_conveyor/CircularQueue.cpp) | `"CircularQueue.h"` |
| [src/module1_conveyor/CommandStack.cpp](../src/module1_conveyor/CommandStack.cpp) | `"CommandStack.h"` |
| [src/module1_conveyor/ConveyorSystem.cpp](../src/module1_conveyor/ConveyorSystem.cpp) | `"ConveyorSystem.h"` |
| [src/module2_dispatch/DispatchSystem.cpp](../src/module2_dispatch/DispatchSystem.cpp) | `"DispatchSystem.h"` |
| [src/module2_dispatch/HashTable.cpp](../src/module2_dispatch/HashTable.cpp) | `"HashTable.h"` |
| [src/module2_dispatch/MinHeap.cpp](../src/module2_dispatch/MinHeap.cpp) | `"MinHeap.h"` |
| [src/shared/Demos.cpp](../src/shared/Demos.cpp) | `"Demos.h"` |
| [src/shared/Game.cpp](../src/shared/Game.cpp) | `"Game.h"`, `"Renderer.h"`, `"Ui.h"`, `<cmath>` |
| [src/shared/Level.cpp](../src/shared/Level.cpp) | `"Level.h"` |
| [src/shared/Renderer.cpp](../src/shared/Renderer.cpp) | `"Renderer.h"`, `"Ui.h"`, `<cmath>` |
| [src/shared/Screens.cpp](../src/shared/Screens.cpp) | `"Renderer.h"`, `"Ui.h"`, `<cmath>` |
| [src/shared/Simulation.cpp](../src/shared/Simulation.cpp) | `"Simulation.h"` |
| [src/shared/Ui.cpp](../src/shared/Ui.cpp) | `"Ui.h"`, `<cmath>`, `<string>` |

The project headers group related declarations:

- `Config.h`, `Parcel.h`, `Level.h`: settings, parcel records and level layouts.
- The four DSA headers: our queue, stack, heap and hash-table classes.
- `ConveyorSystem.h`, `DispatchSystem.h`, `Simulation.h`: routing, tracking/loading and the combined game logic.
- `Demos.h`, `Game.h`, `Renderer.h`, `Ui.h`: practice operations, the application, drawing and shared Raylib helpers.

## C++ syntax to be able to explain

These are the main syntax choices to review before explaining the code. Some come from the supplied labs or Rig Builder; others may still be unfamiliar.

| Syntax or concept | Plain meaning and location |
|---|---|
| `#ifndef`, `#define`, `#endif` | Header guards: declare a header's contents once per source file. These are preprocessor commands, not includes. Same pattern as Rig Builder. |
| `new`, `new[]`, `delete`, `delete[]`, destructors | Allocate/free nodes or arrays. Each owning DSA class cleans up its own storage. Covered by dynamic-memory labs. |
| `Type(const Type& other)`, `operator=` | Copy constructor / copy assignment. Allocate independent storage so two objects do not accidentally share a deleted array or node chain. Lab 01 covers this. |
| `this`, `&other`, `*this` | Current object's pointer, address of the source object, current object itself. `this == &other` handles assigning an object to itself. `return *this` returns the assignment result. |
| `const Parcel&`, `const Cell&` | A read-only reference to an existing record: avoid copying it and prevent this function from changing it. `Cell&` without const allows changes to the original. |
| `HeapEntry& output` | An output reference: `extract`/`peek` write an entry into the caller's variable and return a bool indicating success. |
| A method ending in `const` | It promises not to change that object's ordinary members. Used by getters and drawing access. |
| Two `find` methods | Const overloading: ordinary code can edit a found parcel; read-only drawing gets a `const Parcel*`. Both use the same index-search function. |
| `nullptr`, `->`, `&record` | No pointer, access a pointed-to record, address of a record. The hash table returns a temporary pointer or nullptr. |
| `enum class` and `::` | Named choices such as `ParcelStatus::Ready`. The prefix keeps names from different enums separate. This is C++ syntax, not a library include. |
| `namespace Config`, `namespace Ui` | Group settings/functions under a name, such as `Config::Tile` or `Ui::text`. `std::` similarly identifies standard-library names. `using namespace Ui` lets the two drawing files omit the `Ui::` prefix. |
| `namespace { ... }` | File-local helpers in level/graphics source files; they are only visible in that file. |
| `inline` | Lets a small function be defined in a shared header, such as `cellAt`. It does not promise that the compiler will speed it up. |
| `static` renderer methods | Call a drawing function as `Renderer::draw(game)` without creating a Renderer object. These functions read the Game passed to them. |
| Default arguments | For example `CircularQueue(int capacity = Config::InputCapacity)`: use this value when no argument is supplied. |
| `Demos::Demos() : queue(...), heap(...), table(...)` | Construct member objects with specific capacities before entering the constructor body. This avoids constructing then replacing their allocations. |
| `{...}` record initialization and `{}` | Fill fields in their declared order, or use defaults/zero initialization. `ready.insert({id, deadline, sequence})` creates one HeapEntry; `ParcelMotion{}` resets visual state. |
| `condition ? first : second` | A short if/else that chooses a value. Used for small selections and many drawing choices; some graphics expressions nest these. |
| `(int)`, `(float)`, `(Destination)` | Explicit number/enum conversion, mainly for grid positions and Raylib arguments. These are not pointer casts. `0.5f` is a float literal. |
| `++i`, `i++`, chained assignments | Both increments add one when used as loop statements. In `nextSequence++`, the old number is used first. `a = b = 0` sets both to zero. |
| `%`, `&&`, `\|\|`, `!`, `break`, `continue` | Remainder for wraparound; boolean and/or/not; leave a loop; skip to its next iteration. `if (pointer)` means the pointer is not null. |
| `std::to_string`, `.c_str()`, Raylib `TextFormat` | Build readable labels. `TextFormat` uses placeholders such as `%i` for an integer and `%.1f` for a number with one decimal place. Use its returned text immediately. |

## Algorithms and game logic to study

### Heap and hash table

The heap's array represents a binary tree. At index `i`, the parent is `(i - 1) / 2`, and the children are `2 * i + 1` and `2 * i + 2`. Insertion swaps upward; extraction moves the last entry to the root, then swaps downward. Earlier deadlines win, with arrival sequence breaking ties. A full array doubles in size and copies its entries.

The hash table starts at `id % capacity` and tries following buckets after a collision. Deletion leaves a tombstone so a search can continue past that slot. Before a new insertion would make occupied/deleted slots reach 70%, the table grows and reinserts its records. `usedSlots * 10 >= capacity * 7` checks that threshold using integers; `usedSlots` includes the proposed insertion.

Rehashing deletes the old bucket array. Other structures keep parcel IDs and look up records when needed; they must not keep an old pointer across an insertion that could grow the table.

### Movement and parcel status

`occupancy[]` remembers cells occupied before movement. `claimed[]` records empty target cells already taken in this pass, so two parcels cannot choose the same cell. Changing the scan's starting position each tick avoids always checking one merge branch first. These are ordinary arrays and loops.

The registry holds the main parcel record. Queue/cell/heap entries hold its ID at different stages. Finished records remain available for display; leftover references are removed before further movement or loading. Checking for a finished status prevents scoring or penalizing a parcel twice.

### Frames, animation and fonts

The game advances logically every 0.22 seconds while drawing targets 60 frames per second. An accumulator stores time until the next tick. Animation reaches each tick boundary before the next logical move, preventing jumps when a frame crosses that boundary.

Parcel positions interpolate between cell centers: `elapsed / duration` gives travel progress. Junction arrows turn toward their chosen direction, belts keep their scroll offsets, and each outcome creates feedback once. These visual changes do not replace DSA operations.

Fonts load after the Raylib window opens and unload before it closes. Paths start beside the EXE so VS Code and double-click launches find the same files. If a font file is missing, the default Raylib font is available.

### Limits and build tools

The shipped levels generate at most 60 parcels. `MaxParcels` uses Level 4's total for fixed visual arrays. If another level's total is raised above that value, raise this bound too. The spawn guard prevents writing past the arrays but would otherwise stop extra arrivals.

Indexed DSA getters assume valid indices; callers stay within size/capacity. There is no custom recovery system for allocation failure or arbitrary malformed levels.

CMake, `build.bat`, VS Code settings and the Windows CI workflow build the program. CMake's FetchContent downloads Raylib, then the source files compile and link into one game. Use the whole-project build rather than single-file Run Code.

## F1–F3 practice demos

**F1–F3 are practice demonstrations.** They use separate real instances of our classes. Opening one runs the first three insertions immediately, so there is data to inspect without another key press. The footer buttons also open them. Click **Next operation** or press **Enter** to continue. Restart sample/Backspace restores the three-entry example; Close/Esc returns. The live game stays frozen while a demo is open.

- **F1:** the first six steps enqueue #101–#106. Step 7 rejects a full queue; steps 8–9 dequeue; steps 10–11 wrap rear around to slots 0 and 1. Later steps empty the queue and demonstrate underflow.
- **F2:** the first six steps insert deadlines 90, 30, 60, 30, 10, 50. #205 becomes the root because its deadline is 10. Later steps extract entries and show stable ordering of equal deadlines.
- **F3:** IDs 1, 8 and 15 collide in seven buckets. Continue to search, delete #8, search across its tombstone, reuse that bucket and trigger a rehash. The caption reports each actual operation.

## Reading the live sidebar

The sidebar uses live gameplay data. Its input queue contains only parcels waiting to enter that input, so it often empties quickly. Parcels already on the belt have been dequeued. Q or clicking the queue heading changes the inspected input. Braking an input cell while the game is running can make arrivals wait in its queue.

Queue slots use the destination color of the actual parcel record. `lastEntered[]` remembers the last ID transferred onto each input belt; it is shown separately as **Last to belt**, never counted as still waiting. H switches between **Help** and **Factory info**.

`Game::inspectParcel` checks the mouse against each visible parcel's drawn rectangle. It stores only the selected ID and a three-second display timer. The fixed sidebar card looks up that ID in the hash table to show current destination/type/status/deadline. It cannot cover a conveyor or dock. This uses ordinary loops, arrays and Raylib rectangle hit testing; it adds no library includes or game rules.

| Live display | When it fills or empties |
|---|---|
| Input queue | Fills while parcels wait to enter a belt; empties as they enter. A parcel already moving on the belt is no longer queued. |
| Dispatch heap | Holds correctly routed dock arrivals waiting for the shared loader. It may empty quickly when the loader keeps up. |
| Hash count | Counts all generated records, including finished parcels, until restart. |
| Undo stack | Fills when you pause and click a junction. Z removes one change; the next simulation tick clears the rest. |

For example, eight hash records with empty queue/heap buffers is valid: those parcels may already be moving or finished.

## Suggested reading order

1. `Parcel.h`, `Config.h`, then the queue and linked stack.
2. Heap and hash classes, with F1–F3 open alongside the source.
3. `DispatchSystem.cpp`, `ConveyorSystem.cpp`, then `Simulation.cpp`.
4. `main.cpp`, `Game.cpp`, `Renderer.cpp`, `Screens.cpp` and `Ui.cpp`.

Use the demos alongside the source when reviewing these topics. The [report](REPORT.md) records the implementation's origin and the checks completed so far.
