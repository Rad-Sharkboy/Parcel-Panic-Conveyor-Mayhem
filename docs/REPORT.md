# Parcel Panic — short project report

**Course:** Data Structures and Algorithms

**Tools:** C++17, Raylib, CMake, VS Code and MinGW-w64

**Target:** Windows 11

## Aim

Build a playable sorting game that uses the structures taught in class. The player changes conveyor junctions to guide parcels to matching docks before their deadlines.

## Main features

Four levels add rush waves, fragile parcels, recycling and a slower loader. Players can brake belts, pause to plan, undo junction changes and inspect parcels by hovering. F4 skips levels for demonstrations.

## Where the DSA is used

| Structure | Job |
|---|---|
| Circular queue | Hold arrivals until their input belt is free. |
| Linked stack | Undo paused junction changes in reverse order. |
| Min-heap | Load waiting dock parcels by earliest deadline. |
| Hash table | Find and update parcel records by ID. |

The implementations use ordinary arrays, linked nodes and loops. Deep copies follow Lab 01. F1–F3 use the same classes with separate sample data; each opens with three entries.

The structures are not equally important in every level. Input queues often empty quickly. In automated winning runs, peak heap sizes were 1, 1, 3 and 22 across the four levels. Deadline priority changed the order in the later levels. An array could also store this game's small sequential ID range; hashing is partly an educational choice. F3 deliberately creates collisions that those runs did not encounter.

## Code organization

Conveyor/routing, tracking/dispatch, and game/graphics are separate modules. The game rules work without Raylib. Graphics read those rules and draw smooth movement between steps.

## Current checks and limits

Cloud checks covered the custom structures, complete winning runs, movement, pause/undo, demos and mouse/keyboard controls. A Windows EXE was also cross-compiled. Development downloads are built by the Windows workflow; its actual run status is shown in GitHub Actions. Native Windows gameplay and human difficulty feedback still need local review.

There is no sound, save system or high-score storage yet. Development releases are unfinished versions.

The initial prototype was generated with Codex and revised with user feedback. The team plan describes intended work; it is not a claim of completed student contributions.
