# Academic project report

| Project | Details |
|---|---|
| Name | Parcel Panic: Conveyor Mayhem |
| Course | Undergraduate BSCS Data Structures and Algorithms |
| Language and tools | C++17, Raylib 5.5, CMake, VS Code and MinGW-w64 |
| Target | Windows 11, 64-bit |
| Status | Playable development build; local Windows playtesting is still needed |

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

The planned two-member split assigns the first two modules separately and the shared work jointly. This is a study/review plan, not a record of completed student contributions. The starting implementation was generated with Codex. See [TEAM_PLAN.md](TEAM_PLAN.md).

Each tick advances time, attempts an arrival, checks deadlines and the loader, then moves and feeds conveyors when movement is due. Dock arrivals wait for a later loader check. Movement uses an occupancy snapshot and a claimed-target array: an occupied cell cannot receive another parcel during the same pass. Rotating the scan's starting point avoids always favoring one merge branch. These rules prevent two parcels claiming one cell or moving twice because of loop order.

## 5. Levels and presentation

| Level | Challenge | Arrivals | Delivery quota | Score target |
|---|---|---:|---:|---:|
| Sort School | Two colors, two merging inputs and introductory junctions | 20 | 12 | 1,200 |
| Rush Junction | Three inputs, three colors and rush waves | 38 | 23 | 2,300 |
| Fragile Freight | Crusher bypass, express/fragile parcels and recycling | 48 | 29 | 2,800 |
| Mayhem Mode | Vertical sorter, three inputs, an input bypass and a slower loader | 60 | 36 | 3,600 |

Left click changes a junction; right click brakes a cell; Space pauses. Destination colors and letters identify docks, while extra markers identify parcel types. Moving belts, lifetime bars, particles, rewards, combo text and warnings show what is happening. Raylib shapes and bundled Open Sans fonts supply the graphics; no external art or audio download is required. `Config.h` keeps tuning values together.

Graphics target 60 FPS while logical ticks take 0.22 seconds. Fixed arrays and ordinary loops track animation. Parcel positions move smoothly between cell centers, belt offsets survive braking, and junction arrows turn from their visible angle. Tick-boundary timing prevents sudden jumps. Outcome locations and rewards are saved per parcel so simultaneous events each show the correct feedback. Pause and demos freeze the live world.

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

## 6. Verification and observed DSA use

The graphical game compiled and rendered in the cloud, with automated checks of game logic, animation and input. A MinGW cross-build also produced a Windows x64 executable. The [native Windows release build](https://github.com/Rad-Sharkboy/Parcel-Panic-Conveyor-Mayhem/actions/runs/39208548487) then succeeded using `build.bat` on GitHub Actions.

The published [v0.1.0-dev.1 pre-release](https://github.com/Rad-Sharkboy/Parcel-Panic-Conveyor-Mayhem/releases/tag/v0.1.0-dev.1) ZIP was downloaded and checked for integrity. It includes the EXE, bundled fonts/licenses and play instructions; the executable imports Windows system DLLs rather than requiring MinGW compiler DLLs. These checks confirm compilation and packaging. Native Windows 11 gameplay, VS Code/GDB debugging, display scaling and human difficulty evaluation still need local review.

### Does each structure matter in play?

The queue gives FIFO admission and the stack directly supports undo. The heap matters when ready parcels compete for the loader. The hash table centralizes record lookup, although a simple indexed array could also serve this small sequential ID range; hashing is an educational choice here.

In one automatically routed winning run per level, every input queue peaked at one waiting parcel. Peak ready-heap sizes were **1, 1, 3 and 22** across levels 1–4. Later levels included shipments whose deadline order differed from arrival order, so heap priority affected dispatch. Those runs had no hash collisions because the sequential IDs fit the growing table; F3 deliberately chooses colliding IDs to show probing.

These measurements describe those routes, not every possible playthrough. Early levels show less queue/heap backlog. The display improvements do not hold parcels artificially or change level timing.

## 7. Limits and next review

Arrivals are deterministic: there are no alternate random seeds. Conservative movement leaves visible spacing. An express delivery extends queued/on-belt deadlines without changing priorities already in the ready heap. There is no sound, saved game or persistent high-score system. Automated winning routes support the initial balance but do not replace human playtesting.

Next, play all four levels on Windows and record confusing labels, learning time, difficulty, and whether brakes/rushes are useful. Possible later improvements include alternative color palettes, scalable text, optional sound and saved results. The final submission should record actual student contributions and local playtest results.

## References

- [Architecture](ARCHITECTURE.md), [class interfaces](INTERFACES.md), [complexity](COMPLEXITY.md), [code guide](CODE_GUIDE.md), [team plan](TEAM_PLAN.md)
- [Raylib 5.5](https://github.com/raysan5/raylib/releases/tag/5.5), [CMake](https://cmake.org/documentation/), [MSYS2/MinGW](https://www.msys2.org/)
