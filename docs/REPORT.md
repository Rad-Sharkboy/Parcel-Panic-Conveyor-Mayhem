# Parcel Panic: Conveyor Mayhem — Preliminary Academic Report

**Project type:** undergraduate BSCS data structures semester prototype
**Language and framework:** C++17, Raylib 5.5, CMake
**Development environment:** Windows 11, VS Code and MinGW-w64
**Repository:** Rad-Sharkboy/PARCEL-PANIC-CONVEYOR-MAYHEM
**Status:** implemented prototype with verified builds; local Windows/human evaluation remains outstanding.

## 1. Abstract

Parcel Panic: Conveyor Mayhem is a two-dimensional factory arcade puzzle game in which the player switches conveyor junctions to guide parcels toward matching docks. Parcels arrive continuously, travel through merging routes and compete for a shared loading station. Express deadlines, fragile handling, recycling, rush waves and a congestion meter create time pressure. Four custom data structures are necessary to this behavior: circular queues buffer arrivals, a hash table stores authoritative parcel information, a binary min-heap schedules ready shipments, and a linked stack implements planning undo. The project separates deterministic simulation from Raylib presentation so algorithms and gameplay invariants can be validated without opening a graphical window.

The educational objective is to explain why a data structure fits a real operation, identify its ownership and complexity, and demonstrate edge cases visibly. The practical objective is a playable prototype with responsive controls and clear progression rather than a static algorithm visualization. Actual compilation, runtime rendering and automated input checks were performed in the cloud; no Windows execution or human playtest is implied by those results.

## 2. Problem and requirements

A conveyor sorter combines ordering, lookup, priority and reversal problems. Input machines must preserve parcel order while downstream cells are occupied. Conveyor cells need efficient access to a parcel's destination, deadline, type and lifecycle without owning copies that can diverge. A shared loader must select the earliest deadline among parcels that have already reached a correct dock. Finally, the player should be able to reverse planning edits without attempting to rewind physical movement.

The shipped game therefore imposes explicit rules. A cell contains at most one active parcel. A parcel belongs to exactly one active location: an input queue, a conveyor cell or dispatch staging. Only matching docks admit parcels into the heap. A deadline miss is terminal and penalized once. Fragile parcels are destroyed on a crusher, while damaged parcels require recycling. Victory requires both the delivery quota and score target. All parcels eventually terminate, or the level stops on congestion or the safety time bound. F4 skips directly between levels for instructor demonstrations and does not create a legitimate victory.

## 3. Data structure design and justification

### Circular queue

Each input uses a manually allocated integer array with front, rear, size and capacity fields. Rear identifies the next insertion position. Enqueue and dequeue update their indices modulo capacity and reject full/empty operations without modifying the queue. This bounded design is appropriate because an input machine has a finite buffer; a full queue delays generation rather than silently dropping a parcel. IDs, not parcel copies, are stored. Both operations take O(1) time, including overflow rejection. Clearing takes O(capacity).

### Binary min-heap

Correctly routed parcels enter a dynamically growing array heap. Each entry contains parcel ID, absolute deadline and arrival sequence. Comparison orders first by deadline and then by sequence, making equal-deadline behavior deterministic and teachable. Insertion sifts upward; extraction replaces the root with the last entry and sifts downward. Peek is O(1); extraction is O(log n). Insertion is amortized O(log n), with an O(n) worst-case allocation/copy step when the backing array doubles. The loader's cadence is separate from conveyor movement, so reaching a dock does not guarantee immediate shipment.

### Open-addressed hash table

The hash table owns parcel records. Positive integer IDs map to buckets using modulo arithmetic; collisions advance by linear probing. Buckets distinguish empty, occupied and tombstone states. A tombstone preserves a probe chain after deletion, while duplicate keys update the existing record without increasing size. The table grows based on occupied-plus-tombstone load to avoid long probes under normal use.

Lookup, update and deletion have expected O(1) cost but O(n) worst-case probing. Rehashing is expected O(n) and can be O(n²) under pathological collisions; insertion is therefore not advertised as constant worst-case. Rehashing invalidates record pointers, so other structures store IDs and retrieve only temporary pointers. This ownership rule avoids dangling pointers when the table grows.

### Linked command stack

Each stack node stores a junction index and its previous outgoing choice. A paused edit pushes a command; Z pops the most recent command and restores that choice. Push/pop are O(1), excluding allocator cost, and clearing is O(depth). The stack is cleared at the next simulation tick. It therefore reverses planning decisions while respecting the boundary between configuration and completed movement. It is not a physical rewind system.

All four classes destroy their arrays or nodes and disable copying to prevent shallow-copy double deletion. No STL container substitutes for these implementations.

## 4. Architecture and module responsibilities

There are three logical modules despite the planned two-person team. Conveyor and Routing contains the queue, stack and occupancy engine. Parcel Tracking and Dispatch contains the registry, heap, deadlines, scoring and loader. Shared Game and Presentation coordinates level generation and fixed-step simulation, handles input and menus, and draws the factory and demos.

The intended student split assigns Module 1 to Member 1 and Module 2 to Member 2; Module 3 and integration are joint responsibilities. This describes a future learning/review arrangement. The initial implementation was generated with Codex, and no student contribution or authorship is fabricated. Both students must understand all modules before a viva.

The core compiles independently of Raylib. A logical tick first advances time, attempts generation, checks deadlines and the loader, then moves conveyors on the level cadence. Movement snapshots occupancy and claims target cells before committing transfers. Rotating iteration priority gives merging branches a deterministic opportunity to progress. Cells occupied at the beginning of a pass cannot receive another parcel during that pass. This conservative spacing prevents moving a parcel twice, swapping through occupied cells or resolving a collision differently because of render timing.

## 5. Gameplay and educational presentation

Sort School introduces two colors, two merging inputs and two controllable junctions. Rush Junction adds a third stream, three destinations and rush timing. Fragile Freight adds an explicit safe bypass around a crusher and four dock destinations, including recycling. Mayhem Mode changes the main sorting flow to a vertical spine with three incoming lanes, an input bypass and tighter dispatch capacity.

The player can switch junctions with the left mouse button, brake a cell with the right mouse button and pause with Space. Cardboard parcels carry color-coded destination labels, with additional markers for fragile, express and damaged types. Continuous conveyor rails, moving arrows, lifetime bars, particles, floating rewards, combo text and warnings communicate system state. Graphics use Raylib primitives and bundled Open Sans fonts; no separate art or sound assets are needed. Configurable constants centralize gameplay, DSA and animation tuning.

The presentation uses fixed arrays, ordinary loops and linear interpolation. It advances animation at simulation tick boundaries, keeps belt offsets during brakes, and turns junction arrows from their visible angle even when the player clicks rapidly or undoes a switch. Pause and demos hold the factory animation. Outcomes retain their actual cell and points so several events in one tick can each produce feedback. These changes affect presentation without changing the four data-structure implementations or level rules.

F1–F3 open isolated repeatable instances of the actual custom structures. Enter performs one operation; Backspace resets. The queue demo exposes physical array slots and front/rear wraparound. The heap demo displays the real array and tree, including stable deadline ties and extraction restructuring. The hash demo deliberately collides IDs 1, 8 and 15, performs successful/missing searches, creates/reuses a tombstone and triggers rehashing. These demonstrations hold the live simulation rather than corrupting it. The live dashboard separately reads the real game structures.

## 6. Build and runtime verification

The graphical game compiled and rendered with Raylib 5.5 in the cloud. A separate MinGW-w64 cross-build produced a 64-bit Windows game executable with only Windows system DLL imports. The build also copies the bundled fonts beside the executable. This confirms Windows-target compilation and linking; the executable still needs native Windows 11 play verification.

The Windows workflow builds the same project through `build.bat`. Native Windows execution, VS Code/GDB integration, display scaling and human difficulty evaluation remain part of local review. Hosted CI results should be read from the actual workflow run.

## 7. Limitations and future evaluation

This is a first prototype. Its deterministic generator helps repeat demonstrations but does not yet support alternate seeds. Conservative movement leaves visible spacing and simplifies safety. Express dispatch extends queued/on-belt lifetimes without modifying priorities already in the ready heap. No sound, persistent high scores or save games are implemented. The preliminary balance is supported by automated route solving, not a broad user study.

The next evaluation should ask students unfamiliar with the layout to play each level, measure learning time, record confusion between type and destination, and assess whether rushes and brakes are useful under realistic reactions. Further improvements could add accessibility palettes, scalable text, optional procedural sound and local result persistence. A final academic submission should include the students' own explanations, genuine contribution records and the results of local Windows and human playtesting.

## 8. Supporting material

Implementation contracts: [INTERFACES](INTERFACES.md). Data flow: [ARCHITECTURE](ARCHITECTURE.md). Complexity: [COMPLEXITY](COMPLEXITY.md). Intended team plan: [TEAM_PLAN](TEAM_PLAN.md). Official dependency references: [Raylib 5.5](https://github.com/raysan5/raylib/releases/tag/5.5), [CMake](https://cmake.org/documentation/), and [MSYS2/MinGW](https://www.msys2.org/).
