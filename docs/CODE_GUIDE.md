# Code guide: includes, class examples and unfamiliar parts

This guide describes the project's own C++ files. Raylib is an external graphics library, downloaded by CMake. Its internal headers and implementation are not part of the custom DSA code.

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

The names tell you the dependency: `Config.h` provides constants; `Parcel.h` parcel records; `Level.h` grid/layout records; the four DSA headers their custom classes; `ConveyorSystem.h` routing; `DispatchSystem.h` tracking/loading; `Simulation.h` the logical game; `Demos.h` practice operations; `Game.h` the application; `Renderer.h` drawing functions; and `Ui.h` reusable Raylib helpers.

## C++ syntax to be able to explain

These remain because they have a small, useful job. Some appear in the supplied labs or Rig Builder; that does not mean everyone in the class is already comfortable with them.

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

## Algorithms and game logic that still need study

- **Heap:** an array represents a binary tree. Parent is `(i - 1) / 2`; children are `2 * i + 1` and `2 * i + 2`. Insertion swaps upward; removing the root swaps downward after moving the last entry to the root. If deadlines match, the smaller arrival sequence wins. Growth allocates a doubled array and copies entries.
- **Hash table:** `id % capacity` selects the first bucket. Linear probing tries following buckets. Deleting leaves a tombstone so a search can continue past it. Before used/deleted slots reach 70%, a larger array is allocated and live entries are inserted again. `usedSlots * 10 >= capacity * 7` expresses that threshold without floating-point arithmetic.
- **Pointers after rehash:** the old bucket array is deleted. Store parcel IDs in other structures and look up the record when needed; never retain an old record pointer across insertion that might grow the table.
- **Movement pass:** `occupancy[]` remembers which cells were occupied before movement; `claimed[]` records which empty targets have already been taken this pass. Two parcels cannot claim one cell. Starting the scan at a different position each tick prevents always favoring one merge branch. These are plain arrays and loops, but the reason for them needs explanation.
- **Parcel status:** the hash table is the main record of each parcel. Queue/cell/heap entries store its ID at different stages. Finished parcels remain as records for display; leftover references are cleaned up before further movement/loading. A terminal-state check prevents scoring or penalizing one parcel twice.
- **Ticks versus frames:** the logical game advances every 0.22 seconds, while graphics target 60 frames per second. An accumulator tracks time until the next tick. Animation advances to each tick boundary before the next logical move, which prevents jumps when a frame crosses that boundary.
- **Animation:** positions interpolate toward cell centers; elapsed time divided by travel duration gives progress. Junctions turn toward the chosen angle, belts retain scroll offsets, and outcomes create their effects once. This is presentation code and does not replace a DSA operation.
- **Font lifetime:** Raylib fonts are loaded after creating the window and unloaded before closing it. Paths are resolved beside the executable so VS Code and double-click launches find the same files. A default Raylib font is available if a font file is missing.

The shipped levels generate at most 60 parcels. `MaxParcels` currently uses Level 4's total for fixed visual arrays. If another level is increased beyond that value, increase this bound too. The spawn guard prevents writing past the arrays but would otherwise stop extra arrivals. Indexed DSA getters assume valid indices; they are called within their size/capacity bounds. There is no custom recovery system for allocation failure or arbitrary malformed levels.

CMake, `build.bat`, VS Code launch settings and the Windows CI workflow are build tools, not runtime DSA. CMake's FetchContent downloads Raylib; separate source files are compiled and linked into one game. The code is not a single-file Run Code project.

## Why an inspector can be empty

**F1–F3 are practice demonstrations.** They use separate real instances of our classes and intentionally start empty. Click **Next operation** or press **Enter**. Reset/Backspace starts over; Close/Esc returns. The live game stays frozen while a demo is open.

- **F1:** the first six steps enqueue #101–#106. Step 7 rejects a full queue; steps 8–9 dequeue; steps 10–11 wrap rear around to slots 0 and 1. Later steps empty the queue and demonstrate underflow.
- **F2:** the first six steps insert deadlines 90, 30, 60, 30, 10, 50. #205 becomes the root because its deadline is 10. Later steps extract entries and show stable ordering of equal deadlines.
- **F3:** IDs 1, 8 and 15 collide in seven buckets. Continue to search, delete #8, search across its tombstone, reuse that bucket and trigger a rehash. The caption reports each actual operation.

**The sidebar is live gameplay data.** Its input queue contains only parcels waiting to enter that input, so it often empties quickly. Parcels already on the belt have been dequeued. Q or clicking the queue heading changes the inspected input. Braking an input cell while the game is running can make arrivals wait in its queue.

The heap contains correctly routed dock arrivals waiting for the shared loader. If it loads them quickly, there may be no backlog to display. The hash count includes all generated records, even after delivery/failure. The undo stack fills only when you pause and click a cyan junction; Z pops one edit, and the next simulation tick clears the history. In the supplied screenshot, eight hash records with empty waiting buffers was a valid state.

## Suggested reading order

1. `Parcel.h`, `Config.h`, then the queue and linked stack.
2. Heap and hash classes, with F1–F3 open alongside the source.
3. `DispatchSystem.cpp`, `ConveyorSystem.cpp`, then `Simulation.cpp`.
4. `main.cpp`, `Game.cpp`, `Renderer.cpp`, `Screens.cpp` and `Ui.cpp`.

The syntax and algorithms listed above are the parts to review before explaining the program in class. Matching the examples cannot guarantee an instructor's assessment; the report retains the implementation's origin.
