# Two-member plan, three logical modules

This is the intended future division of responsibility. The starting prototype was generated with Codex; the labels below do not assert human authorship or completed student work. Both members must learn and defend the complete program.

| Planned owner | Module | Responsibilities | Review partner |
|---|---|---|---|
| Member 1 | Conveyor and Routing | Circular queue, linked stack, grid movement, merge arbitration, braking, planning-only undo | Member 2 |
| Member 2 | Parcel Tracking and Dispatch | Hash registry, heap ordering, tombstones/rehash, statuses, deadlines, scoring and loader | Member 1 |
| Joint | Shared Game and Presentation | Raylib, menus, fixed-step loop, four levels, effects, demos, integration tests, documentation and balancing | Both |

## Suggested learning milestones

1. Trace parcel #1 from generator to registry, input queue, conveyor, dock, ready heap and terminal dispatch. Draw the states and identify the owning module.
2. Member 1 demonstrates full/empty queue behavior, wraparound and LIFO undo. Member 2 demonstrates heap ties and a tombstone probe chain. Each partner then explains the other's algorithms.
3. Build locally and run all tests in Debug and Release. Read one failure-producing test in each module and explain what broken implementation it would detect.
4. Play all four layouts, identify a safe route for every color/type, and test brakes at merges. Record human balancing feedback separately from automated results.
5. Use feature branches and peer-reviewed pull requests for changes. Keep interface and complexity documents synchronized with implementation.

## Five-minute viva outline

- 0:00–0:45: launch Sort School, explain letters, switch a junction and show the queue-driven input.
- 0:45–1:30: pause, switch twice, press Z twice; explain why movement is not rewound.
- 1:30–2:30: F1/Enter to fill and wrap a queue; F2/Enter to compare equal deadlines; F3/Enter to create collisions and a tombstone.
- 2:30–3:45: F4 to show the three remaining layouts, safe fragile bypass, damaged recycling and rush warnings. F4 always works; Shift+F4 goes back.
- 3:45–5:00: show the passing C++ test output, ownership rules and honest worst-case complexity. Discuss Windows / MinGW build verification and remaining human playtesting.

No invented student names, contribution percentages or fabricated commits are included.
