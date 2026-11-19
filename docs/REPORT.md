# Academic project report — V2

| Project | Details |
|---|---|
| Name | Parcel Panic: Conveyor Mayhem |
| Course | Undergraduate BSCS Data Structures and Algorithms |
| Language and tools | C++17, Raylib 5.5, CMake, VS Code and MinGW-w64 |
| Target | Windows 11, 64-bit |
| Status | V2 release build; local Windows playtesting is still needed |

## 1. Aim

Parcel Panic is a factory sorting game. The player switches conveyor junctions to guide parcels toward matching docks while managing deadlines, fragile freight, damaged parcels, rush waves and congestion.

The project connects four DSA topics to game actions: queues hold arrivals, a hash table tracks parcels, a min-heap chooses the next ready shipment, and a linked stack undoes planning changes. The aim is both a playable game and a program whose operations, memory ownership and complexity can be explained in class. Game logic is separate from Raylib drawing, so it can also be checked without opening a window.

## 2. Main rules

- Each cell holds at most one active parcel. An active parcel belongs to an input queue, a conveyor cell or dispatch staging.
- Only a parcel reaching its matching dock enters the ready heap. All docks share one loader.
- Wrong docks, missed deadlines and crushed fragile parcels cause failure and one penalty per parcel. Damaged parcels need the recycling dock.
- A full input queue delays the next arrival. Waiting parcels keep their FIFO order.
- Victory needs both the delivery quota and score target after all generated parcels finish. A full jam meter or maximum time limit ends the run in failure.
- Paused junction changes can be undone; completed movement cannot. F4 skips levels for demonstrations without awarding victory.

## 3. Why each data structure is used

### Circular queue: waiting at an input

Each input has a fixed-capacity dynamic integer array. It stores parcel IDs, with `front = 0`, `rear = -1` and `count = 0` initially. Enqueue advances rear using modulo before writing; rear therefore means the last inserted slot. Dequeue returns the oldest ID and advances front. An empty dequeue returns `-1`; a full enqueue returns `false`, leaving the queue unchanged.

FIFO fits arrivals that must wait for belt space. Enqueue, dequeue and peek take O(1); clearing the slots takes O(capacity). The queue does not grow, matching the input's limited buffer.

### Min-heap: choosing the next shipment

A correct dock arrival adds its ID, deadline and arrival sequence to a binary min-heap. Earlier deadlines have priority; equal deadlines keep arrival order. Insertion moves an entry upward. Extraction replaces the root with the last entry, then moves it downward.

The shared loader has its own loading interval, so several correctly routed parcels may compete for dispatch. Peek takes O(1); extraction takes O(log n). Insertion is O(log n) amortized, but an individual insertion can take O(n) when the full array doubles. Reaching a dock does not mean immediate delivery.

### Hash table: tracking parcel records

The registry owns each parcel's destination, type, status and deadline. `id % capacity` chooses its first bucket; linear probing tries following buckets after a collision. Buckets can be empty, occupied or tombstones. Deletion leaves a tombstone so searches can still reach later colliding IDs. Inserting an existing ID updates it without increasing the count.

The table grows before a new insertion would make occupied-plus-deleted slots reach 70%. With normal hashing, lookup/update/deletion are expected O(1); a bad collision pattern can require checking the whole table. Rehashing scans the arrays and reinserts records, potentially taking O(capacity + n²). Full bounds are in [COMPLEXITY.md](COMPLEXITY.md).

Other structures keep IDs and look up records when needed. They cannot retain pointers across a table resize because the old array is deleted. Finished records stay available until restart.

### Linked stack: undoing planning changes

Each node holds the changed junction and its old outgoing choice. A paused switch pushes at the top; Z pops the latest change and restores it. This last-in, first-out order directly matches undo. Push/pop take O(1), excluding allocation cost; clearing takes O(depth).

Every simulation tick clears the history. Undo therefore applies to planning changes, not past parcel movement.

All four structures use manual arrays or linked nodes, destructors and independent deep copies, following the supplied labs. There are no prebuilt stack, queue, heap or map replacements. [CODE_GUIDE.md](CODE_GUIDE.md) lists every include and the C++ concepts to study.

## 4. Modules and update order

| Module | Main responsibility |
|---|---|
| Conveyor and Routing | Queue admission, stack undo, movement, merges, junctions and brakes |
| Parcel Tracking and Dispatch | Hash records, heap order, statuses, deadlines, scoring and the loader |
| Shared Game and Presentation | Four levels, simulation timing, Raylib input, menus, graphics and demos |

The project uses two feature modules plus shared presentation work. [Sabtain](https://github.com/Sabtainn) contributed Module 2, Parcel Tracking and Dispatch, and shared responsibility for Module 3, Game Integration and Presentation. The starting implementation was generated with Codex. The study and viva plan is included below.

Each tick advances time, attempts an arrival, checks deadlines and the loader, then moves and feeds conveyors when movement is due. Dock arrivals wait for a later loader check. Movement uses an occupancy snapshot and a claimed-target array: an occupied cell cannot receive another parcel during the same pass. Rotating the scan's starting point avoids always favoring one merge branch. These rules prevent two parcels claiming one cell or moving twice because of loop order.

## 5. Levels and presentation

| Level | Challenge | Arrivals | Delivery quota | Score target |
|---|---|---:|---:|---:|
| Sort School | Two colors, two merging inputs and introductory junctions | 20 | 12 | 1,200 |
| Rush Junction | Three inputs, three colors and rush waves | 38 | 23 | 2,300 |
| Fragile Freight | Crusher bypass, express/fragile parcels and recycling | 48 | 29 | 2,800 |
| Mayhem Mode | Vertical sorter, three inputs, an input bypass and a slower loader | 60 | 36 | 3,600 |

Left click changes a junction; right click brakes a cell; Space pauses. Destination colors and letters identify docks, while extra markers identify parcel types. Moving belts, lifetime bars, particles, rewards, combo text and warnings show what is happening. Raylib shapes and bundled Tomorrow fonts supply the graphics; no external art or audio download is required. `Config.h` keeps tuning values together.

The window is 1360 × 765, with the original layout scaled up by 6.25% and larger queue labels. Graphics target 60 FPS while logical ticks take 0.22 seconds. Fixed arrays and ordinary loops track animation. Parcel positions move smoothly between cell centers, belt offsets survive braking, and junction arrows turn from their visible angle. Tick-boundary timing prevents sudden jumps. Outcome locations and rewards are saved per parcel so simultaneous events each show the correct feedback. Pause and demos freeze the live world.

### Demos and the live sidebar

F1–F3 use separate instances of the actual custom structures and open with three real entries already inserted. Enter/Next operation continues; Backspace/Restart sample restores the starting example; Esc closes it. Footer buttons also open them.

| Display | What it demonstrates |
|---|---|
| F1 queue | Physical array slots, front/rear, full/empty rejection and wraparound |
| F2 heap | Array/tree layout, smaller deadlines moving upward, equal-deadline order and extraction |
| F3 hash table | IDs 1, 8 and 15 colliding, searches, tombstone deletion/reuse and rehashing |
| Live input queue | Parcels currently waiting at the selected input; Q or the heading changes input |
| Last to belt | The latest real departure, kept visible separately from the waiting queue |
| Live heap/hash/stack | Ready shipments, all generated records, and paused junction changes |

Queue slots use parcel destination colors. Hovering a moving parcel opens a fixed sidebar card with its ID, type, destination, status and remaining time. H switches Help/Factory info. Empty live buffers can be normal: parcels may move onto the belt or through the loader quickly. The demos remain populated independently of live gameplay.

## 6. Completion review and verification

Initial audit found working DSA implementations and reachable routes, but an almost invisible shared loader, technical information in ordinary gameplay, immediate special-type introductions, no live stepping inspector and no checked-in test target. The supplied proposal and master prompt guided this pass; lecture source files were unavailable.

The Windows GCC 15.2.0 / CMake 4.3.4 Release build succeeds through `build.bat`. The generated `build` folder can be deleted and recreated; Raylib downloads automatically on the first build. Raylib's unused MOD decoder is disabled; its custom-config conditional definitions are made consistent. No compiler warning suppression was added to game code.

During V2 development, a local verification harness checked capacity/wraparound/deep copies, heap ties/growth, hash collisions/tombstones/rehash, stack LIFO/inspection, deadlines, wrong docks, fragile hazards, duplicate completion prevention, brakes, backpressure, jams, restart, level switching, graph reachability and one-cell occupancy. Test-only routing verified that every required dock has a safe route from every input and that the shipped graphs contain no cycles. All four scripted runs won. The harness is intentionally excluded from the public V2 source folder.

| Level | Delivered / failed | Peak ready heap | Dispatches choosing a later arrival by deadline |
|---|---|---:|---:|
| Sort School | 20 / 0 | 1 | 0 |
| Rush Junction | 38 / 0 | 10 | 16 |
| Fragile Freight | 48 / 0 | 12 | 10 |
| Mayhem Mode | 58 / 2 | 17 | 18 |

These are deterministic routing-policy results, not human playtest scores. Input queues peak at one in those runs; separate blockage/brake tests verify real buffering and backpressure. Parcels are not held artificially to fill a display. Sequential live IDs can avoid collisions; F3 remains the explicitly labeled collision/tombstone sample.

The native Windows title/start button and first briefing were inspected interactively. Additional gameplay, inspector, pause, help and selection layouts were rendered from real simulation states and visually checked. This does not certify every keyboard shortcut or Windows display-scale setting. Windows CI is configured to build and package the V2 release after its version tag is pushed.

## 7. Preserved behavior and limits

All four custom algorithms, manual storage, public operations and Rule of Three implementations remain. Only `CommandStack::commandAt(depth) const` adds read-only traversal for inspection; push/pop are unchanged. The existing four layouts, names, three-module split, Tomorrow fonts, 1360 x 765 window and F1-F5 shortcuts remain.

Loader timings are now 6/9/9/7 ticks. Level 3 gates fragile, express and damaged generation using `Config.h` thresholds. Score/quota commitments remain. The new shared-bay panel, input staging, hover route previews, failure messages and skippable briefings explain these rules. I opens real structures; Enter advances an actual tick; S/J/Z demonstrate real planning changes in the stack tab. Closing inspection remains paused.

Remaining review: human difficulty and complete keyboard/mouse playtesting across all levels, display-scaling checks and VS Code/GDB debugging. Automated winning routes cannot establish enjoyment or learning time. There is no audio, save system or persistent high score. Arbitrary board-size changes are not validated. No new claim of compatibility with unseen lecture code is made.

## 8. Contribution and viva plan

Both contributors should understand the whole program before the viva.

| Contributor | Area | Work to understand and review |
|---|---|---|
| Project team | Conveyor and Routing | Circular queue, linked stack, grid movement, merge decisions, brakes and planning undo |
| [Sabtain](https://github.com/Sabtainn) | Parcel Tracking and Dispatch | Hash registry, heap ordering, tombstones/rehash, parcel statuses, deadlines, scoring and loader |
| Both contributors | Shared Game and Presentation | Raylib, menus, tick timing, four layouts, animation, demos, module connections, documentation and balance |

Both contributors should review the other module before the viva.

### Suggested order

1. Trace parcel #1 through the registry, queue, conveyor, dock, heap and final dispatch. Identify its status and responsible module at each step.
2. Show queue full/empty behavior and wraparound, stack undo, equal heap deadlines and searching past a hash tombstone. Then swap topics and explain the other member's structures.
3. Build both Release and Debug on Windows. Trace movement/loading and check full buffers, wrong docks and expired deadlines. Explain the results from the code.
4. Play all four levels. Find a route for each destination/type, try brakes at merges and record confusing or unbalanced parts.
5. Review changes together. Use specific commit messages, and update interface and complexity notes when behavior changes.

### Five-minute viva outline

| Time | Demonstration |
|---|---|
| 0:00–0:45 | Launch Sort School, explain parcel letters, switch a junction and show an input queue |
| 0:45–1:30 | Pause, switch twice, press Z twice; explain why past movement is not undone |
| 1:30–2:30 | I/Tab/Enter to inspect live queue/heap/hash/stack and step a tick; F3 for the labeled collision/tombstone sample |
| 2:30–3:45 | F4 through the remaining layouts; show the fragile bypass, recycling and rush warnings. Shift+F4 goes back. |
| 3:45–5:00 | Explain module boundaries, who owns memory, worst-case complexity, verified Windows builds and remaining local playtesting |

Keep contribution records factual; do not invent names, percentages or completed work.

## 9. References

- [Architecture](ARCHITECTURE.md), [complexity](COMPLEXITY.md), [code guide](CODE_GUIDE.md)
- [Raylib 5.5](https://github.com/raysan5/raylib/releases/tag/5.5), [CMake](https://cmake.org/documentation/), [MSYS2/MinGW](https://www.msys2.org/)
