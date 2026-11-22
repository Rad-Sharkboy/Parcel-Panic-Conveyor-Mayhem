#include "Inspector.h"
#include "Game.h"
#include "Ui.h"

using namespace Ui;
namespace {
const Rectangle Close{1068, 155, 150, 36};
const Rectangle Step{863, 155, 190, 36};
const Rectangle Previous{63, 622, 110, 30};
const Rectangle NextPage{187, 622, 110, 30};
const Rectangle Search{64, 263, 220, 40};
const Rectangle Lookup{298, 263, 140, 40};
const Rectangle Switch{65, 268, 250, 38};
const Rectangle Undo{329, 268, 180, 38};
const Rectangle NextJunction{523, 268, 270, 38};

Rectangle tabBounds(int i) { return {64 + i * 190.0f, 209, 178, 34}; }
Rectangle slotBounds(int i, float y, int columns, float width, float height) {
    return {64 + (i % columns) * width, y + (i / columns) * height, width - 7, height - 7};
}

// Compute binary tree coordinates for the first 15 nodes of the array-backed MinHeap.
Vector2 nodePosition(int index) {
    int depth = 0, first = 0, width = 1;
    while (index >= first + width) {
        first += width;
        width *= 2;
        depth++;
    }
    return {64 + 740.0f * (index - first + 0.5f) / width, 290 + depth * 52.0f};
}

int itemCount(const Game& g) {
    if (g.inspector.tab == 0) {
        return g.simulation.conveyor.inputs[g.inspectedLane].getCapacity();
    }
    if (g.inspector.tab == 1) {
        return g.simulation.dispatch.ready.size();
    }
    if (g.inspector.tab == 2) {
        return g.simulation.dispatch.registry.getCapacity();
    }
    return g.simulation.conveyor.history.size();
}

int pageSize(int tab) {
    return tab == 0 ? 16 : tab == 1 ? 12 : tab == 2 ? 40 : 6;
}

const char* activityText(const Activity& event) {
    switch (event.operation) {
    case Operation::Enqueue:
        return TextFormat("Enqueue #%i -> input %i", event.id, event.detail + 1);
    case Operation::Dequeue:
        return TextFormat("Dequeue #%i -> belt %i", event.id, event.detail + 1);
    case Operation::SkipQueued:
        return TextFormat("Dequeue stale #%i", event.id);
    case Operation::Push:
        return TextFormat("Push cell %i, old choice %i", event.id, event.detail);
    case Operation::Pop:
        return TextFormat("Pop: cell %i restored to %i", event.id, event.detail);
    case Operation::ClearUndo:
        return TextFormat("Tick cleared %i planning changes", event.id);
    case Operation::HeapInsert:
        return TextFormat("Heap insert #%i, deadline %i", event.id, event.detail);
    case Operation::Dispatch:
        return TextFormat("Extract-min #%i -> shipped", event.id);
    case Operation::SkipReady:
        return TextFormat("Extract stale #%i (no score)", event.id);
    case Operation::Register:
        return TextFormat("Hash insert #%i", event.id);
    case Operation::Rehash:
        return TextFormat("Rehash: %i -> %i buckets", event.detail, event.id);
    }
    return "";
}
} // namespace

void Inspector::input(Game& g) {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_I) || clicked(Close) || clicked(InspectorToggle)) {
        open = false;
        g.inspectedParcel = selectedId;
        g.parcelInfoTime = 3;
        return; // Remain paused so the inspected state is still visible on the floor.
    }

    // Digits belong exclusively to parcel search; tabs are selected by click or Tab.
    for (int i = 0; i < 4; i++) {
        if (clicked(tabBounds(i))) {
            tab = i;
            page = 0;
        }
    }
    if (IsKeyPressed(KEY_TAB)) {
        tab = (tab + 1) % 4;
        page = 0;
    }
    if (clicked(Step) || IsKeyPressed(KEY_ENTER)) {
        g.stepOnce();
    }
    if (IsKeyPressed(KEY_Z)) {
        g.simulation.conveyor.undo();
    }
    if (tab == 0 && IsKeyPressed(KEY_Q) && g.simulation.conveyor.layout.inputCount > 0) {
        g.inspectedLane = (g.inspectedLane + 1) % g.simulation.conveyor.layout.inputCount;
        page = 0;
    }

    if (clicked(Previous) || IsKeyPressed(KEY_LEFT)) {
        page--;
    }
    if (clicked(NextPage) || IsKeyPressed(KEY_RIGHT)) {
        page++;
    }
    int totalItems = itemCount(g);
    int maxPage = totalItems > 0 ? (totalItems - 1) / pageSize(tab) : 0;
    if (page > maxPage) {
        page = maxPage;
    }
    if (page < 0) {
        page = 0;
    }

    int start = page * pageSize(tab);
    if (tab == 0) {
        const CircularQueue& queue = g.simulation.conveyor.inputs[g.inspectedLane];
        for (int i = 0; i < 16 && start + i < queue.getCapacity(); i++) {
            if (clicked(slotBounds(i, 333, 8, 93, 93))) {
                selectedId = queue.getSlot(start + i);
            }
        }
    } else if (tab == 1) {
        const MinHeap& heap = g.simulation.dispatch.ready;
        for (int i = 0; i < heap.size() && i < 15; i++) {
            Vector2 p = nodePosition(i);
            if (clicked({p.x - 32, p.y - 20, 64, 42})) {
                selectedId = heap.getEntry(i).id;
            }
        }
        for (int i = 0; i < 12 && start + i < heap.size(); i++) {
            if (clicked(slotBounds(i, 510, 6, 124, 47))) {
                selectedId = heap.getEntry(start + i).id;
            }
        }
    } else if (tab == 2) {
        int character = GetCharPressed();
        while (character > 0) {
            if (character >= '0' && character <= '9' && searchId < 100000) {
                searchId = searchId * 10 + character - '0';
            }
            character = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            searchId /= 10;
        }
        const HashTable& registry = g.simulation.dispatch.registry;
        if (clicked(Lookup) || IsKeyPressed(KEY_L)) {
            const Parcel* parcel = registry.find(searchId); // Actual const hash lookup.
            selectedId = parcel ? parcel->id : 0;
            if (searchId > 0) {
                page = registry.hash(searchId) / 40;
            }
        }
        start = page * 40;
        for (int i = 0; i < 40 && start + i < registry.getCapacity(); i++) {
            if (clicked(slotBounds(i, 341, 8, 93, 51))) {
                const Bucket& bucket = registry.getBucket(start + i);
                if (bucket.state == BucketState::Occupied) {
                    selectedId = searchId = bucket.parcel.id;
                }
            }
        }
    } else {
        int totalJunctions = g.simulation.conveyor.layout.junctionCount;
        if (totalJunctions > 0) {
            if (clicked(NextJunction) || IsKeyPressed(KEY_J)) {
                junction = (junction + 1) % totalJunctions;
            }
            if (clicked(Switch) || IsKeyPressed(KEY_S)) {
                g.simulation.conveyor.switchJunction(g.simulation.conveyor.layout.junctions[junction], true);
            }
        }
        if (clicked(Undo)) {
            g.simulation.conveyor.undo();
        }
    }
}

void Inspector::draw(const Game& g) const {
    const Simulation& s = g.simulation;
    const HashTable& registry = s.dispatch.registry;
    DrawRectangle(0, 135, 1280, 540, Fade(Background, 0.96f));
    box({40, 137, 1200, 532}, Panel, 0.04f);
    text("LIVE DSA INSPECTOR", 64, 155, 27, Cyan, true);
    text(TextFormat("Paused at tick %i | real game state", s.tick), 410, 164, 17, Muted);
    button("Tick +1 / Enter", Step, true);
    button("Close / I", Close);

    const char* labels[4] = {"Circular queue", "Min-heap", "Hash table", "Linked stack"};
    for (int i = 0; i < 4; i++) {
        box(tabBounds(i), tab == i ? Cyan : Raised);
        centered(labels[i], tabBounds(i), 18, tab == i ? Background : White, true);
    }
    text("Tab: change view / click a parcel entry.", 856, 215, 16, Muted);
    DrawLine(830, 256, 830, 650, Border);

    int start = page * pageSize(tab);
    if (tab == 0) {
        const CircularQueue& activeLaneQueue = s.conveyor.inputs[g.inspectedLane];
        text(TextFormat("Input %i / Q changes lane     %i of %i slots used", g.inspectedLane + 1,
                        activeLaneQueue.size(), activeLaneQueue.getCapacity()),
             64, 265, 22);
        text(TextFormat("Front %i   Rear %i (last insertion)   FIFO / O(1)", activeLaneQueue.getFront(),
                        activeLaneQueue.getRear()),
             64, 303, 17, Muted);
        for (int i = 0; i < 16 && start + i < activeLaneQueue.getCapacity(); i++) {
            int index = start + i;
            int id = activeLaneQueue.getSlot(index);
            const Parcel* p = registry.find(id);
            Rectangle slot = slotBounds(i, 333, 8, 93, 93);
            box(slot, Raised);
            text(TextFormat("[%i]", index), slot.x + 9, slot.y + 7, 14, Muted);
            centered(id ? TextFormat("#%i", id) : "empty", {slot.x, slot.y + 25, slot.width, 28}, 20,
                     p ? destinationColor(p->destination) : Muted, true);
            const char* marker = index == activeLaneQueue.getFront() && index == activeLaneQueue.getRear()
                                     ? "F + R"
                                 : index == activeLaneQueue.getFront() ? "FRONT"
                                 : index == activeLaneQueue.getRear()  ? "REAR"
                                                                       : "";
            centered(marker, {slot.x, slot.y + 57, slot.width, 24}, 14, Cyan);
        }
        text(activeLaneQueue.isFull()    ? "FULL: new arrivals wait until a slot is free."
             : activeLaneQueue.isEmpty() ? "Empty is normal: these parcels may already be on belts."
                                         : "Only waiting IDs are here; dequeue admits the front to its belt.",
             64, 542, 18, Gold);
        text("Wraparound reuses slots. No parcel is held just to populate this view.", 64, 577, 17, Muted);
    } else if (tab == 1) {
        const MinHeap& activeHeap = s.dispatch.ready;
        text(TextFormat("%i entries | deadline first, then arrival sequence", activeHeap.size()), 64, 257, 18);
        for (int i = 1; i < activeHeap.size() && i < 15; i++) {
            DrawLineEx(nodePosition((i - 1) / 2), nodePosition(i), 2, Border);
        }
        for (int i = 0; i < activeHeap.size() && i < 15; i++) {
            const HeapEntry& entry = activeHeap.getEntry(i);
            Vector2 p = nodePosition(i);
            box({p.x - 34, p.y - 15, 68, 43}, i == 0 ? Cyan : Raised);
            centered(TextFormat("#%i", entry.id), {p.x - 34, p.y - 14, 68, 22}, 18,
                     i == 0 ? Background : White, true);
            centered(TextFormat("d:%i", entry.deadline), {p.x - 34, p.y + 7, 68, 19}, 13,
                     i == 0 ? Background : Gold);
        }
        if (activeHeap.isEmpty()) {
            text("No dock arrivals yet. Advance ticks or route parcels first.", 90, 355, 20, Muted);
        }
        text("Tree: first 15 nodes. Array below is paged; siblings need not be sorted.", 64, 485, 16, Muted);
        for (int i = 0; i < 12 && start + i < activeHeap.size(); i++) {
            const HeapEntry& entry = activeHeap.getEntry(start + i);
            Rectangle slot = slotBounds(i, 510, 6, 124, 47);
            box(slot, Raised);
            centered(TextFormat("[%i] #%i  d:%i", start + i, entry.id, entry.deadline), slot, 14, White);
        }
    } else if (tab == 2) {
        box(Search, Background);
        text(TextFormat("ID: %i", searchId), 78, 274, 21, Cyan);
        button("Lookup / L", Lookup);
        text("Type digits / Backspace", 453, 277, 16, Muted);

        // Read-only linear probing trace highlighting visited slots and Tombstone hops.
        int home = registry.hash(searchId), probes = 0, stop = home;
        bool found = false;
        if (searchId > 0) {
            for (int probeStep = 0; probeStep < registry.getCapacity(); probeStep++) {
                stop = (home + probeStep) % registry.getCapacity();
                const Bucket& b = registry.getBucket(stop);
                probes++;
                if (b.state == BucketState::Occupied && b.parcel.id == searchId) {
                    found = true;
                    break;
                }
                if (b.state == BucketState::Empty) {
                    break;
                }
            }
        }
        text(TextFormat("Read-only trace: %i %% %i = %i | %i probes | %s", searchId,
                        registry.getCapacity(), home, probes, found ? "FOUND" : "MISSING"),
             64, 315, 17, Gold);
        for (int i = 0; i < 40 && start + i < registry.getCapacity(); i++) {
            int index = start + i;
            const Bucket& b = registry.getBucket(index);
            Rectangle slot = slotBounds(i, 341, 8, 93, 51);
            box(slot, b.state == BucketState::Occupied ? Raised : Background);
            int distance = (index - home + registry.getCapacity()) % registry.getCapacity();
            if (distance < probes) {
                DrawRectangleLinesEx(slot, 2, index == stop ? Gold : Cyan);
            }
            text(TextFormat("[%i]", index), slot.x + 6, slot.y + 3, 13, Muted);
            centered(b.state == BucketState::Occupied    ? TextFormat("#%i", b.parcel.id)
                     : b.state == BucketState::Tombstone ? "DELETED"
                                                         : "empty",
                     {slot.x, slot.y + 20, slot.width, 21}, 16,
                     b.state == BucketState::Occupied ? destinationColor(b.parcel.destination) : Muted);
        }
        text("Live records are retained; F3's sample demonstrates deletion/collisions.", 64, 601, 16, Muted);
    } else {
        int cell = s.conveyor.layout.junctions[junction];
        button(TextFormat("S  Switch junction %i", junction + 1), Switch, true);
        button("Z  Undo / pop", Undo);
        button("J  Select next junction", NextJunction);
        text(TextFormat("Selected cell %i (%i,%i), choice %i | %i saved commands", cell,
                        cell % Config::Columns, cell / Config::Columns, s.conveyor.layout.cells[cell].choice,
                        s.conveyor.history.size()),
             64, 320, 18, Gold);
        for (int i = 0; i < 6 && start + i < s.conveyor.history.size(); i++) {
            SwitchCommand command = s.conveyor.history.commandAt(start + i);
            Rectangle row{96, 355 + i * 38.0f, 665, 31};
            box(row, start + i == 0 ? Cyan : Raised);
            centered(TextFormat("%s  cell %i    restore choice %i    -> next node",
                                start + i == 0 ? "TOP / next undo" : "saved command", command.cell,
                                command.oldChoice),
                     row, 17, start + i == 0 ? Background : White);
        }
        if (s.conveyor.history.isEmpty()) {
            text("S pushes a real planning change. Z pops and restores it.", 96, 380, 21, Muted);
        }
        text("Tick +1 clears history: planning undo never rewinds parcel movement.", 64, 594, 17, Muted);
    }

    button("< Page", Previous);
    button("Page >", NextPage);
    text(TextFormat("%i / %i", page + 1, (itemCount(g) > 0 ? (itemCount(g) - 1) / pageSize(tab) : 0) + 1),
         320, 628, 16, Muted);

    // Side panel: live HashTable parcel record + filtered operation history.
    const Parcel* selected = registry.find(selectedId);
    text("REAL PARCEL RECORD", 856, 263, 18, Cyan, true);
    if (selected) {
        const char* kinds[4] = {"Normal", "Express", "Fragile", "Damaged"};
        const char* statuses[9] = {"Created", "Queued", "On conveyor", "At dock", "Ready at loader",
                                  "Dispatched", "Wrong dock", "Broken", "Expired"};
        text(TextFormat("#%i  /  %s  /  %s", selected->id, destinationLetter(selected->destination),
                        kinds[(int)selected->kind]),
             856, 299, 24, destinationColor(selected->destination));
        text(statuses[(int)selected->status], 856, 333, 20);
        text(TextFormat("Cell %i  |  Last dock/event %i", selected->cell, selected->eventCell), 856, 365, 16,
             Muted);
        text(TextFormat("Deadline %i  /  now %i", selected->deadline, s.tick), 856, 393, 17, Gold);
    } else {
        text("Select an entry, or search an ID.", 856, 305, 19, Muted);
        text("This is the simulation's hash record.", 856, 339, 16, Muted);
    }
    DrawLine(856, 434, 1215, 434, Border);
    text("RECENT REAL OPERATIONS", 856, 451, 16, Cyan, true);

    const ActivityLog& log = tab == 0 || tab == 3 ? s.conveyor.activity : s.dispatch.activity;
    int shown = 0;
    for (int i = 0; i < log.count && shown < 6; i++) {
        const Activity& event = log.entries[i];
        bool stackEvent = event.operation == Operation::Push || event.operation == Operation::Pop ||
                          event.operation == Operation::ClearUndo;
        bool hashEvent = event.operation == Operation::Register || event.operation == Operation::Rehash;
        if ((tab == 0 && stackEvent) || (tab == 3 && !stackEvent)) {
            continue;
        }
        if ((tab == 1 && hashEvent) || (tab == 2 && !hashEvent)) {
            continue;
        }
        text(activityText(event), 856, 481 + shown * 27.0f, 16, shown == 0 ? White : Muted);
        shown++;
    }
    if (shown == 0) {
        text("No operations yet. Step or plan a switch.", 856, 489, 16, Muted);
    }
}