#include "Renderer.h"
#include "Ui.h"
#include <algorithm>
#include <cmath>

using namespace Ui;
namespace {
void miniature(const LevelDefinition& level, Rectangle bounds) {
    float tile = std::min(bounds.width / Config::Columns, bounds.height / Config::Rows);
    for (int i = 0; i < Config::Cells; ++i) {
        const Cell& cell = level.cells[i];
        if (cell.kind == CellKind::Empty) continue;
        Vector2 p{bounds.x + (i % Config::Columns + 0.5f) * tile, bounds.y + (i / Config::Columns + 0.5f) * tile};
        for (int branch = 0; branch < (cell.kind == CellKind::Junction ? 2 : 1); ++branch) {
            int target = cell.next[branch];
            if (target < 0) continue;
            Vector2 q{bounds.x + (target % Config::Columns + 0.5f) * tile, bounds.y + (target / Config::Columns + 0.5f) * tile};
            DrawLineEx(p, q, 4, Border);
        }
        if (cell.kind == CellKind::Dock) DrawRectangleRec({p.x - 4, p.y - 4, 8, 8}, destinationColor(cell.dock));
        else if (cell.kind == CellKind::Junction) DrawCircleV(p, 4, Cyan);
        else if (cell.kind == CellKind::Hazard) DrawCircleV(p, 4, Gold);
    }
}
void titleFactory(float clock) {
    box({510, 98, 685, 518}, Panel, 0.08f);
    text("A small factory. A very busy shift.", 550, 133, 28);
    text("Match the labels. Keep the parcels moving.", 550, 179, 20, Muted);
    BeginScissorMode(530, 230, 645, 210);
    // An illustrative loop; no independent gameplay or advanced effects.
    DrawLineEx({545, 348}, {1165, 348}, 66, Border);
    DrawLineEx({545, 348}, {1165, 348}, 52, Raised);
    DrawLineEx({869, 348}, {869, 255}, 66, Border);
    DrawLineEx({869, 348}, {869, 255}, 52, Raised);
    for (int i = 0; i < 35; ++i) {
        float x = 536 + std::fmod(clock * 35 + i * 20, 640.0f);
        DrawLineEx({x, 325}, {x, 371}, 2, Border);
    }
    DrawCircle(869, 348, 34, Cyan);
    DrawCircle(869, 348, 29, Panel);
    arrow({869, 348}, 0, 18, Cyan);
    DrawRectangleRec({848, 245, 42, 18}, destinationColor(Destination::Red));
    for (int i = 0; i < 5; ++i) {
        float x = 510 + std::fmod(clock * 56 + i * 151, 755.0f);
        parcel({x, 348}, static_cast<Destination>(i % 3), ParcelKind::Normal, 1, 1.6f);
    }
    EndScissorMode();
    const char* labels[3] = {"Route the right color", "Protect fragile freight", "Build delivery combos"};
    for (int i = 0; i < 3; ++i) {
        box({550, 454 + i * 43.0f, 25, 25}, i == 1 ? Gold : Cyan);
        centered(TextFormat("%i", i + 1), {550, 454 + i * 43.0f, 25, 25}, 16, Background, true);
        text(labels[i], 592, 455 + i * 43.0f, 19, i == 1 ? Gold : White);
    }
}
}
void Renderer::menu(const Game& g) {
    if (g.screen == Screen::Menu) {
        text("THE SORTING SHIFT STARTS HERE", 84, 92, 16, Cyan, true);
        text("Parcel", 77, 135, 72);
        text("Panic", 77, 207, 72, Gold);
        text("Conveyor Mayhem", 84, 294, 23, Muted);
        button("Start shift  /  Enter", Play, true);
        button("Choose a level", Select);
        button("How to play", Help);
        button("Quit", Quit);
        titleFactory(g.clock);
        text("Four shifts. Queues, heaps, hashing and a little panic.", 550, 633, 17, Muted);
    } else if (g.screen == Screen::Select) {
        text("Choose your shift", 70, 75, 44);
        text("Start anywhere. Each floor has its own routing challenge.", 73, 143, 22, Muted);
        text("Press 1-4 to jump in", 73, 186, 17, Cyan);
        const char* lessons[4] = {"Learn the routes", "Handle merging lanes", "Keep fragile parcels safe", "Survive the busiest shift"};
        for (int i = 0; i < Config::Levels; ++i) {
            LevelDefinition level = makeLevel(i);
            Rectangle card = levelCard(i);
            bool hover = CheckCollisionPointRec(GetMousePosition(), card);
            box(card, hover ? Raised : Panel, 0.08f);
            text(TextFormat("SHIFT %02i", i + 1), card.x + 20, card.y + 17, 14, Cyan, true);
            text(level.name, card.x + 20, card.y + 43, 25);
            miniature(level, {card.x + 20, card.y + 89, 235, 138});
            text(lessons[i], card.x + 20, card.y + 235, 16, White);
            text(TextFormat("Deliver %i / %i arrivals", level.tuning.quota, level.tuning.total), card.x + 20, card.y + 266, 16, Muted);
            text(TextFormat("Score target %i", level.tuning.target), card.x + 20, card.y + 291, 16, Gold);
        }
        button("Back  /  Esc", Back);
    } else if (g.screen == Screen::Help) {
        text("A quick guide to the factory", 70, 59, 40);
        text("Match parcel letters to docks, then let the shared loader ship them.", 73, 116, 21, Muted);
        const char* keys[10] = {"Left click", "Right click", "Space / Esc", "Z (paused)", "H", "R / F5", "F1", "F2", "F3", "F4 / Shift+F4"};
        const char* actions[10] = {"Switch a cyan junction", "Brake a belt cell temporarily", "Pause, plan and resume", "Undo the last junction change", "Toggle the live DSA panel", "Restart this level", "Circular queue demo", "Deadline min-heap demo", "Hash table and probing demo", "Next / previous level"};
        for (int i = 0; i < 10; ++i) {
            float y = 177 + i * 38.0f;
            text(keys[i], 74, y, 18, Cyan, true);
            text(actions[i], 265, y, 18, White);
        }
        box({735, 169, 470, 397}, Panel, 0.08f);
        text("Read the parcel", 763, 192, 25);
        for (int i = 0; i < 4; ++i) parcel({798.0f + i * 117, 269}, static_cast<Destination>(i), static_cast<ParcelKind>(i), 1, 1.2f);
        text("R / B / Y", 763, 319, 18, Cyan, true);
        text("Match the shipping dock color", 880, 319, 17);
        text("X", 763, 354, 18, destinationColor(Destination::Recycle), true);
        text("Damaged: send to recycling", 880, 354, 17);
        text("F", 763, 389, 18, White, true);
        text("Fragile: avoid the crusher", 880, 389, 17);
        text("!", 763, 424, 18, Gold, true);
        text("Express: a shorter deadline", 880, 424, 17);
        DrawLine(763, 468, 1177, 468, Border);
        text("Reach BOTH the delivery quota and score target.", 763, 486, 17, Gold);
        text("Wrong docks and expired parcels break combos.", 763, 520, 17, Muted);
        button("Back  /  Esc", Back);
    }
}
void Renderer::results(const Game& g) {
    DrawRectangle(0, 3, Config::WindowWidth, 679, Fade(Background, 0.88f));
    box({350, 184, 580, 397}, Panel, 0.08f);
    bool won = g.simulation.outcome == Outcome::Won;
    if (won) {
        // Simple falling rectangles, clipped to the result card.
        BeginScissorMode(351, 185, 578, 394);
        for (int i = 0; i < 24; ++i) {
            float x = 360 + std::fmod(i * 67.0f, 555.0f);
            float y = 185 + std::fmod(g.clock * (26 + i % 4 * 8) + i * 43.0f, 390.0f);
            DrawRectanglePro({x, y, 4, 8}, {2, 4}, g.clock * 65 + i * 21.0f, Fade(i % 2 ? Cyan : Gold, 0.4f));
        }
        EndScissorMode();
    }
    text(won ? "Shift complete!" : "Shift over", 390, 215, 42, won ? Cyan : Gold);
    text(g.simulation.conveyor.layout.name, 393, 276, 22, Muted);
    text(TextFormat("%05i", g.simulation.dispatch.score), 393, 310, 36);
    text("points", 529, 327, 18, Muted);
    text(TextFormat("Delivered %i   /   Failed %i", g.simulation.dispatch.delivered, g.simulation.dispatch.failed), 652, 328, 18, White);
    text(won ? "Quota and score target reached. Ready for the next shift?" : g.simulation.jam >= Config::JamLimit ? "The belts jammed. Try braking one input lane." : "Try again: match the docks and watch the deadlines.", 393, 386, 17, Gold);
    button("Retry  /  F5", Retry);
    button("Next shift  /  F4", Next, true);
    button("Main menu", ResultsMenu);
}
void Renderer::demonstration(const Game& g) {
    const Demos& d = g.demos;
    DrawRectangle(0, 3, Config::WindowWidth, 679, Fade(Background, 0.96f));
    box({60, 70, 1160, 584}, Panel, 0.06f);
    text(d.mode == 1 ? "Circular queue" : d.mode == 2 ? "Deadline min-heap" : "Open-address hash table", 90, 96, 34);
    text("Enter  Next operation     Backspace  Reset     Esc  Close", 90, 150, 20, Cyan);
    text("Game paused", 1043, 153, 16, Muted);
    DrawLine(90, 190, 1188, 190, Border);
    if (d.mode == 1) {
        const CircularQueue& queue = d.queue;
        text(TextFormat("Size %i / %i     Front %i     Rear %i (last insert)", queue.size(), queue.getCapacity(), queue.getFront(), queue.getRear()), 90, 219, 24);
        for (int i = 0; i < queue.getCapacity(); ++i) {
            Rectangle slot{100 + i * 180.0f, 306, 158, 108};
            box(slot, queue.getSlot(i) ? Raised : Background);
            text(TextFormat("SLOT %i", i), slot.x + 18, slot.y + 14, 14, Muted, true);
            centered(queue.getSlot(i) ? TextFormat("#%i", queue.getSlot(i)) : "empty", {slot.x, slot.y + 37, slot.width, 61}, 28, queue.getSlot(i) ? Gold : Muted);
            if (i == queue.getFront()) centered("FRONT", {slot.x, 433, slot.width, 24}, 16, Cyan, true);
            if (i == queue.getRear()) centered("REAR", {slot.x, 462, slot.width, 24}, 16, Gold, true);
        }
        text("Fixed-capacity array. Enqueue and dequeue take O(1); indices wrap with modulo.", 90, 527, 19, Muted);
    } else if (d.mode == 2) {
        const MinHeap& heap = d.heap;
        text(TextFormat("Size %i   /   Capacity %i   /   Earliest deadline first, then arrival order", heap.size(), heap.capacity()), 90, 218, 22);
        for (int i = 0; i < heap.size(); ++i) {
            int depth = i < 1 ? 0 : i < 3 ? 1 : 2;
            int first = depth == 0 ? 0 : depth == 1 ? 1 : 3;
            int count = depth == 0 ? 1 : depth == 1 ? 2 : 4;
            float x = 100 + (i - first + 0.5f) * (600.0f / count);
            float y = 302 + depth * 94.0f;
            if (i > 0) {
                int parent = (i - 1) / 2;
                int parentDepth = parent == 0 ? 0 : 1;
                int parentFirst = parentDepth == 0 ? 0 : 1;
                int parentCount = parentDepth == 0 ? 1 : 2;
                Vector2 p{100 + (parent - parentFirst + 0.5f) * (600.0f / parentCount), 302 + parentDepth * 94.0f};
                float dx = x - p.x, dy = y - p.y;
                float length = std::sqrt(dx * dx + dy * dy);
                dx /= length; dy /= length;
                DrawLineEx({p.x + dx * 34, p.y + dy * 34}, {x - dx * 34, y - dy * 34}, 2, Border);
            }
            DrawCircleV({x, y}, 33, i == 0 ? Color{44, 94, 98, 255} : Raised);
            centered(TextFormat("#%i", heap.at(i).id), {x - 30, y - 25, 60, 28}, 20, White, true);
            centered(TextFormat("d:%i", heap.at(i).deadline), {x - 30, y + 2, 60, 23}, 16, Gold);
        }
        text("ACTUAL ARRAY", 791, 272, 15, Cyan, true);
        for (int i = 0; i < heap.size(); ++i) {
            text(TextFormat("[%i]  #%i   deadline %i   seq %llu", i, heap.at(i).id, heap.at(i).deadline, heap.at(i).sequence), 791, 306 + i * 31.0f, 17, i == 0 ? Gold : White);
        }
        text("Only parcels at a matching dock enter the gameplay heap.", 90, 541, 18, Muted);
    } else {
        const HashTable& table = d.table;
        text(TextFormat("hash(id) = id %% %i     Records %i     Linear probing with tombstones", table.capacity(), table.size()), 90, 219, 22);
        int visible = std::min(table.capacity(), 15);
        for (int i = 0; i < visible; ++i) {
            Rectangle slot{100 + (i / 5) * 365.0f, 269 + (i % 5) * 48.0f, 335, 39};
            const Bucket& bucket = table.bucket(i);
            box(slot, bucket.state == BucketState::Occupied ? Raised : Background);
            text(TextFormat("[%02i]", i), slot.x + 12, slot.y + 8, 17, Muted);
            text(bucket.state == BucketState::Occupied ? TextFormat("#%i   home %i", bucket.parcel.id, table.hash(bucket.parcel.id)) : bucket.state == BucketState::Tombstone ? "Tombstone" : "Empty", slot.x + 80, slot.y + 8, 18, bucket.state == BucketState::Occupied ? Cyan : Muted);
        }
        text("IDs 1, 8 and 15 collide at capacity 7. Every operation reads this actual table.", 90, 541, 18, Muted);
    }
    box({80, 579, 1120, 54}, Background);
    text(TextFormat("STEP %02i", d.step), 98, 596, 16, Gold, true);
    text(d.event.c_str(), 206, 593, 18);
}
