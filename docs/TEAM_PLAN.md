# Two-member study and review plan

This is a planned division of work, not a claim of completed contributions. The starting prototype was generated with Codex. Both members should understand the whole program before the viva.

| Planned owner | Area | Work to understand and review |
|---|---|---|
| Member 1 | Conveyor and Routing | Circular queue, linked stack, grid movement, merge decisions, brakes and planning undo |
| Member 2 | Parcel Tracking and Dispatch | Hash registry, heap ordering, tombstones/rehash, parcel statuses, deadlines, scoring and loader |
| Both | Shared Game and Presentation | Raylib, menus, tick timing, four layouts, animation, demos, module connections, documentation and balance |

Each member reviews the other's module. Names and contribution records can be filled in when that work has actually been done.

## Suggested order

1. Trace parcel #1 through the registry, queue, conveyor, dock, heap and final dispatch. Identify its status and responsible module at each step.
2. Show queue full/empty behavior and wraparound, stack undo, equal heap deadlines and searching past a hash tombstone. Then swap topics and explain the other member's structures.
3. Build both Release and Debug on Windows. Trace movement/loading and check full buffers, wrong docks and expired deadlines. Explain the results from the code.
4. Play all four levels. Find a route for each destination/type, try brakes at merges and record confusing or unbalanced parts.
5. Review changes together. Use specific commit messages, and update interface and complexity notes when behavior changes.

## Five-minute viva outline

| Time | Demonstration |
|---|---|
| 0:00–0:45 | Launch Sort School, explain parcel letters, switch a junction and show an input queue |
| 0:45–1:30 | Pause, switch twice, press Z twice; explain why past movement is not undone |
| 1:30–2:30 | F1/Enter for full/wrapped queues, F2/Enter for equal deadlines, F3/Enter for collisions and a tombstone |
| 2:30–3:45 | F4 through the remaining layouts; show the fragile bypass, recycling and rush warnings. Shift+F4 goes back. |
| 3:45–5:00 | Explain module boundaries, who owns memory, worst-case complexity, verified Windows builds and remaining local playtesting |

Keep contribution records factual; do not invent names, percentages or completed work.
