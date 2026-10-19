# Code guide

Start with the queue and stack, then read the heap, hash table and game loop.

## How this matches the labs

- **Queue:** an array with `front`, `rear`, `count` and `capacity`. Rear starts at -1; enqueue advances it before writing. Dequeue removes from front. `% capacity` wraps an index back to the start.
- **Stack:** linked nodes with `data` and `next`. `top` points to the newest node. Push adds at top; pop removes and deletes top.
- **Heap:** an array. Swap entries with a temporary variable, using loops. Parent is `(i - 1) / 2`; children are `2 * i + 1` and `2 * i + 2`.
- **Hash table:** start at `id % capacity`. If occupied, try the next slot. A deleted marker (tombstone) tells searches to keep going. Growing the table means inserting records again into the new array.

Lab 01 covers the destructors and deep copies used here. Lab 02 covers arrays and resizing; the linked-node and queue patterns follow the later labs and field manual. Heap/hash details still need study when taught. No prebuilt DSA containers are used.

## Includes

| Header | Why it is used |
|---|---|
| `<string>` | Store messages/font paths and convert IDs to text. |
| `<cmath>` | Raylib drawing needs angles, distances, rounding and repeating motion. |
| `"raylib.h"` | Window, input, shapes, fonts and timing. |
| Our own `.h` files | Share our classes and settings between source files. |

There is no direct `<iostream>`, `<climits>` or `<algorithm>` include. Every standard include has a short purpose comment in the code.

<details>
<summary>Every include, file by file</summary>

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

</details>

## Syntax you may need to explain

| Code | Meaning |
|---|---|
| `#ifndef` / `#define` / `#endif` | Prevent a header being declared twice in one source file. Same pattern as Rig Builder. |
| `new[]`, `delete[]`, destructor | Allocate an array and free it when finished. Linked nodes use `new` and `delete`. |
| Copy constructor / `operator=` | Make a separate copy of the data, instead of sharing the same pointer. Lab 01's Rule of Three. |
| `this == &other`, `return *this` | Check whether an object is being assigned to itself; return the current object afterward. |
| `const Parcel&` | Read the original parcel without copying or changing it. Without `const`, a reference can change the original. |
| `HeapEntry& output` | Let the function write its result into the caller's variable. |
| `size() const` / const `find` | Read an object without changing it. The two `find` versions allow editable or read-only access. |
| `nullptr`, `->`, `&record` | No pointer; read a pointed-to field; get a record's address. |
| `enum class` | Named choices, such as `ParcelStatus::Ready`, instead of unexplained numbers. |
| `Config::`, `Ui::`, `std::` | The name before `::` groups related constants/functions. `using namespace Ui` lets drawing code omit that prefix. |
| `namespace { ... }` | Helpers used only inside that source file. |
| `inline` / `static` methods | Inline permits a small function in a shared header. A static drawing method can be called without creating a Renderer object. |
| `queue(6)` after a constructor's `:` | Construct that member object with capacity 6 before the constructor body runs. |
| `{...}`, `{}`, default arguments | Fill fields in order; use default/zero values; use a supplied default when an argument is omitted. |
| `condition ? a : b` | Choose a value with a short if/else. Common in drawing code. |
| `(int)`, `(float)`, `(Destination)` | Convert a value to the required number/enum type. `0.5f` is a float. |
| `++`, `%`, `&&`, `\|\|`, `!` | Add one; remainder; and; or; not. In `nextSequence++`, the old value is used first. |
| `to_string`, `c_str()`, `TextFormat` | Turn a number into text; pass a string to Raylib; format labels using `%i` or `%.1f`. |

## Important ideas behind the code

- **Keep IDs:** resizing the hash table moves its records. Look up a parcel again instead of keeping an old pointer across insertion.
- **One box per cell:** `occupancy[]` remembers the starting positions; `claimed[]` stops two boxes taking one target during a step.
- **Separate rules and drawing:** rules advance every 0.22 seconds. The visible box moves smoothly between cell positions using elapsed time.
- **Simple fixed limits:** visual arrays allow up to 60 parcels. Increase `MaxParcels` too if future levels generate more.
- **Fonts:** load after opening the window; unload before closing it. Keep the font files beside the EXE.

CMake builds all source files together and downloads Raylib. It does not implement any of our data structures.

## Reading the panels

F1–F3 use separate practice objects and open after three real insertions. Next operation/Enter continues; Restart sample/Backspace starts again. F1 shows queue wraparound; F2 shows deadline ordering; F3 shows collisions, deletion and resizing.

The live queue contains only waiting parcels. **Last to belt** remembers an ID that already left. The heap holds matching-dock arrivals waiting for the loader. The hash count includes finished parcels. Pause and switch a junction to fill the undo stack.

H switches Help / Factory info. Hover uses the drawn parcel rectangle, then looks up its ID for the sidebar card. It uses ordinary loops and Raylib hit testing, with a three-second timer.
