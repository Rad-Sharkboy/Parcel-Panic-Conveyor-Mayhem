#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <string>
namespace {
const Color Ink{13, 20, 30, 255}, Panel{23, 34, 47, 255}, Track{48, 65, 79, 255};
const Color White{236, 244, 247, 255}, Muted{143, 164, 181, 255}, Cyan{65, 219, 230, 255}, Gold{255, 203, 85, 255};
Color color(Destination d) {
    switch (d) {
        case Destination::Red: return {255, 103, 117, 255};
        case Destination::Blue: return {94, 161, 255, 255};
        case Destination::Yellow: return Gold;
        case Destination::Recycle: return {90, 222, 155, 255};
    }
    return White;
}
const char* letter(Destination d) { return d == Destination::Red ? "R" : d == Destination::Blue ? "B" : d == Destination::Yellow ? "Y" : "X"; }
void text(const char* label, float x, float y, int size = 20, Color tint = White) { DrawTextEx(GetFontDefault(), label, {x, y}, static_cast<float>(size), 1, tint); }
void box(float x, float y, float width, float height, Color tint = Panel) { DrawRectangleRounded({x, y, width, height}, 0.12f, 5, tint); }
void button(const char* label, float x, float y, float width, float height, bool primary = false) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), {x, y, width, height});
    box(x, y, width, height, hover ? Color{55, 98, 113, 255} : primary ? Color{31, 112, 127, 255} : Track);
    text(label, x + 18, y + (height - 21) / 2, 21, primary ? White : Cyan);
}
void meter(float x, float y, float width, float ratio, Color tint) {
    DrawRectangleRounded({x, y, width, 7}, 0.7f, 3, Track);
    if (ratio > 0) DrawRectangleRounded({x, y, width * std::clamp(ratio, 0.0f, 1.0f), 7}, 0.7f, 3, tint);
}
void arrow(Vector2 position, float angle, float size, Color tint) {
    Vector2 tip{position.x + std::cos(angle) * size, position.y + std::sin(angle) * size};
    Vector2 a{position.x + std::cos(angle + 2.4f) * size, position.y + std::sin(angle + 2.4f) * size};
    Vector2 b{position.x + std::cos(angle - 2.4f) * size, position.y + std::sin(angle - 2.4f) * size};
    DrawTriangle(tip, b, a, tint);
}

}
bool Renderer::clicked(float x, float y, float width, float height) { return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), {x, y, width, height}); }
Vector2 Renderer::center(int cell) {
    return {static_cast<float>(Config::BoardX + (cell % Config::Columns) * Config::Tile + Config::Tile / 2), static_cast<float>(Config::BoardY + (cell / Config::Columns) * Config::Tile + Config::Tile / 2)};
}
void Renderer::background(const Game& g) {
    ClearBackground(Ink);
    for (int y = 0; y < Config::WindowHeight; y += 32) for (int x = 0; x < Config::WindowWidth; x += 32) DrawCircle(x, y, 1, {28, 40, 52, 255});
    DrawRectangle(0, 0, Config::WindowWidth, 7, Cyan);
    DrawRectangle(0, Config::WindowHeight - 39, Config::WindowWidth, 39, Panel);
    text("F4 NEXT LEVEL  /  SHIFT+F4 PREVIOUS  /  F5 RESTART", 28, 694, 17, Cyan);
    text("F1 QUEUE   F2 HEAP   F3 HASH   |   H HUD", 800, 694, 17, Muted);
    if (g.screen == Screen::Playing || g.screen == Screen::Results) {
        text(TextFormat("%02i / %s", g.simulation.level + 1, g.simulation.conveyor.layout.name), 28, 26, 26);
        text(TextFormat("SCORE  %06i", g.simulation.dispatch.score), 580, 26, 28, Gold);
        text(TextFormat("%ix COMBO", g.simulation.dispatch.multiplier()), 945, 26, 28, Cyan);
        const Config::LevelTuning& t = g.simulation.conveyor.layout.tuning;
        text(TextFormat("DELIVER  %i / %i    SCORE TARGET  %i", g.simulation.dispatch.delivered, t.quota, t.target), 28, 70, 19, Muted);
        text(TextFormat("ARRIVALS  %i / %i", g.simulation.generated, t.total), 580, 70, 19, Muted);
        text("JAM", 945, 70, 18, Muted); meter(995, 78, 235, static_cast<float>(g.simulation.jam) / Config::JamLimit, g.simulation.jam > 60 ? color(Destination::Red) : Cyan);
        if (g.simulation.rush() || g.simulation.rushWarning()) text(g.simulation.rush() ? "RUSH WAVE ACTIVE!" : "RUSH WAVE INCOMING!", 360, 105, 24, Gold);
        else text(g.paused ? "PLANNING PAUSE - switch junctions, then Z to undo" : "LMB switch  |  RMB brake  |  SPACE plan  |  Z undo while paused", 28, 107, 18, g.paused ? Gold : Muted);
    }
}
void Renderer::factory(const Game& g) {
    const Simulation& s = g.simulation;
    const LevelDefinition& l = s.conveyor.layout;
    box(Config::BoardX - 8, Config::BoardY - 8, Config::Columns * Config::Tile + 16, Config::Rows * Config::Tile + 12, {17, 27, 37, 255});
    for (int i = 0; i < Config::Cells; ++i) {
        const Cell& c = l.cells[i]; Vector2 p = center(i);
        if (c.kind == CellKind::Empty) { DrawRectangle(static_cast<int>(p.x) - 1, static_cast<int>(p.y) - 1, 2, 2, Track); continue; }
        if (c.kind == CellKind::Junction) {
            for (int branch = 0; branch < 2; ++branch) {
                Vector2 n = center(c.next[branch]);
                DrawLineEx(p, n, 31, branch == c.choice ? Track : Color{29, 45, 55, 255});
            }
        } else if (c.next[0] >= 0 && c.kind != CellKind::Dock) DrawLineEx(p, center(c.next[0]), 30, Track);
        box(p.x - 18, p.y - 18, 36, 36, Track);
        if (c.kind == CellKind::Dock) {
            box(p.x - 22, p.y - 22, 44, 44, color(c.dock));
            box(p.x - 16, p.y - 16, 32, 32, Ink); text(letter(c.dock), p.x - 7, p.y - 12, 26, color(c.dock));
        } else if (c.kind == CellKind::Hazard) {
            box(p.x - 21, p.y - 21, 42, 42, {105, 52, 36, 255});
            text("!", p.x - 5, p.y - 14, 30, Gold);
            for (int n = -14; n <= 14; n += 9) DrawLineEx({p.x + n, p.y + 14}, {p.x + n + 5, p.y + 7}, 3, Gold);
        } else {
            int target = c.next[c.kind == CellKind::Junction ? c.choice : 0];
            if (target >= 0) {
                Vector2 n = center(target); float angle = std::atan2(n.y - p.y, n.x - p.x);
                if (c.kind == CellKind::Junction) {
                    DrawCircleV(p, 21 + g.junctionPulse[i] * 6, Fade(Cyan, 0.12f + g.junctionPulse[i] * 0.3f));
                    DrawCircleLines(static_cast<int>(p.x), static_cast<int>(p.y), 21, Cyan);
                    if (g.junctionPulse[i] > 0) {
                        Vector2 old = center(c.next[1 - c.choice]); float previous = std::atan2(old.y - p.y, old.x - p.x);
                        float diff = angle - previous; while (diff > PI) diff -= 2 * PI; while (diff < -PI) diff += 2 * PI;
                        angle = previous + diff * (1 - g.junctionPulse[i]);
                    }
                    arrow(p, angle, 12, Cyan);
                } else {
                    float slide = std::fmod(g.clock * 20, 20.0f) - 10;
                    arrow({p.x + std::cos(angle) * slide, p.y + std::sin(angle) * slide}, angle, 6, {108, 134, 150, 255});
                }
            }
        }
        if (c.brakeUntil > s.tick) { DrawCircleV(p, 21, Fade(Gold, 0.35f)); text("II", p.x - 7, p.y - 11, 23, Gold); }
        if (i == g.hoverCell && c.kind != CellKind::Empty) DrawRectangleLinesEx({p.x - 23, p.y - 23, 46, 46}, 2, Fade(White, 0.5f));
    }
    for (int i = 0; i < l.inputCount; ++i) {
        Vector2 p = center(l.inputs[i]); DrawCircleV({p.x - 15, p.y - 18}, 11, Cyan); text(TextFormat("%i", i + 1), p.x - 20, p.y - 26, 17, Ink);
    }
    const HashTable& registry = s.dispatch.registry;
    for (int i = 0; i < registry.capacity(); ++i) {
        const Bucket& b = registry.bucket(i); if (b.state != BucketState::Occupied || b.parcel.status != ParcelStatus::OnConveyor) continue;
        const Parcel& parcel = b.parcel; Vector2 p = center(parcel.cell);
        float elapsed = (s.tick - parcel.movedTick) * Config::TickSeconds + g.accumulator;
        float fraction = std::clamp(elapsed / (Config::TickSeconds * l.tuning.moveTicks), 0.0f, 1.0f);
        if (parcel.previousCell >= 0) {
            Vector2 previous = center(parcel.previousCell); p.x = previous.x + (p.x - previous.x) * fraction; p.y = previous.y + (p.y - previous.y) * fraction;
        }
        Color tint = color(parcel.destination);
        box(p.x - 13, p.y - 10, 29, 29, Fade(BLACK, 0.35f));
        box(p.x - 15, p.y - 15, 29, 29, tint);
        DrawRectangle(static_cast<int>(p.x) - 2, static_cast<int>(p.y) - 15, 4, 29, Fade(White, 0.20f));
        text(letter(parcel.destination), p.x - 8, p.y - 10, 20, Ink);
        if (parcel.kind == ParcelKind::Fragile) text("F", p.x + 7, p.y - 22, 15, White);
        else if (parcel.kind == ParcelKind::Express) { if (std::fmod(g.clock, 0.5f) < 0.3f) DrawCircleLines(static_cast<int>(p.x), static_cast<int>(p.y), 20, Gold); text("!", p.x + 8, p.y - 22, 17, Gold); }
        else if (parcel.kind == ParcelKind::Damaged) { DrawLineEx({p.x - 10, p.y - 12}, {p.x + 8, p.y + 10}, 2, Ink); }
        int lifetime = parcel.kind == ParcelKind::Express ? Config::ExpressDeadlineTicks : Config::DeadlineTicks;
        meter(p.x - 14, p.y + 17, 28, static_cast<float>(parcel.deadline - s.tick) / lifetime, parcel.deadline - s.tick < 25 ? color(Destination::Red) : tint);
    }
    for (int i = 0; i < Config::ParticleCount; ++i) { const Particle& p = g.particles[i]; if (p.life > 0) DrawCircleV(p.position, 3, Fade(p.color, p.life / 0.65f)); }
    if (s.tick - s.dispatch.lastEventTick < 8) {
        const Parcel* p = registry.find(s.dispatch.lastEventId);
        if (p && p->previousCell >= 0) {
            Vector2 location = center(p->previousCell);
            text(TextFormat("%+i", s.dispatch.lastPoints), location.x - 20, location.y - 35 - (s.tick - s.dispatch.lastEventTick) * 3.0f, 25, s.dispatch.lastPoints > 0 ? Cyan : color(Destination::Red));
        }
    }
    if (s.dispatch.lastPoints > 0 && s.dispatch.streak > 0 && s.dispatch.streak % Config::ComboStep == 0 && s.tick - s.dispatch.lastEventTick < 7) {
        float rise = (s.tick - s.dispatch.lastEventTick) * 3.0f;
        text(TextFormat("%ix COMBO!", s.dispatch.multiplier()), 270, 608 - rise, 36, Gold);
    }
    if (g.titleTimer > 0 && !g.paused && s.tick < 8) {
        box(180, 330, 530, 90, Fade(Ink, 0.95f)); text(TextFormat("LEVEL %i", s.level + 1), 205, 343, 18, Cyan); text(l.name, 205, 373, 31, White);
    }
}
void Renderer::dashboard(const Game& g) {
    const Simulation& s = g.simulation; const ConveyorSystem& c = s.conveyor;
    box(875, 140, 365, 527);
    text(g.hud ? "LIVE STRUCTURES" : "FACTORY STATUS", 894, 159, 23, Cyan);
    text(TextFormat("TICK %04i  /  %.1fs", s.tick, s.tick * Config::TickSeconds), 894, 194, 18, Muted);
    int cooldown = std::max(0, c.brakeReady - s.tick);
    text(cooldown ? TextFormat("BRAKE recharge: %.1fs", cooldown * Config::TickSeconds) : "BRAKE ready - right click a belt", 894, 223, 18, cooldown ? Gold : Cyan);
    if (g.hud) {
        const CircularQueue& q = c.inputs[0];
        text(TextFormat("QUEUE 1   %i/%i   F:%i R:%i", q.size(), q.capacity(), q.front(), q.rear()), 894, 261, 18);
        for (int i = 0; i < q.capacity(); ++i) {
            float x = 894 + i * 39.0f; box(x, 291, 34, 32, q.slot(i) ? Track : Ink);
            text(q.slot(i) ? TextFormat("%i", q.slot(i)) : "-", x + 6, 298, 17, i == q.front() ? Cyan : Muted);
        }
        HeapEntry root; bool has = s.dispatch.ready.peek(root);
        text(TextFormat("HEAP  ready:%i", s.dispatch.ready.size()), 894, 338, 18);
        text(has ? TextFormat("root #%i  deadline:%i", root.id, root.deadline) : "loader waiting for correct docks", 894, 364, 17, Muted);
        text(TextFormat("HASH  %i records / %i buckets", s.dispatch.registry.size(), s.dispatch.registry.capacity()), 894, 398, 18);
        text(TextFormat("STACK  %i planning commands", c.history.size()), 894, 424, 18, Gold);
    } else {
        text(TextFormat("Delivered: %i   Failed: %i", s.dispatch.delivered, s.dispatch.failed), 894, 275, 21);
        text("R / B / Y = shipping destinations", 894, 320, 17, Muted);
        text("X = damaged / recycle", 894, 348, 17, color(Destination::Recycle));
        text("F = fragile  /  ! = express", 894, 376, 17, Gold);
        text("Avoid crushers with F parcels.", 894, 404, 17, Muted);
    }
    if (g.paused) {
        button("RESUME", 890, 440, 160, 44, true); button("RESTART", 1060, 440, 160, 44);
        button("MAIN MENU", 890, 495, 330, 38);
    } else {
        text("NEXT IN QUEUE", 894, 465, 17, Cyan);
        for (int i = 0; i < c.layout.inputCount; ++i) {
            int id = 0; bool has = c.inputs[i].peek(id); const Parcel* p = has ? s.dispatch.registry.find(id) : nullptr;
            text(p ? TextFormat("IN %i: #%i %s  %s", i + 1, id, letter(p->destination), p->kind == ParcelKind::Fragile ? "FRAGILE" : p->kind == ParcelKind::Express ? "EXPRESS" : "") : TextFormat("IN %i: clear", i + 1), 894, 492 + i * 24.0f, 17, p ? color(p->destination) : Muted);
        }
    }
    text("PAUSE + click + Z = genuine undo", 894, 589, 17, Gold);
    text("F1-F3: step through isolated demos", 894, 615, 16, Muted);
    text("F4 next  |  Shift+F4 back  |  F5 retry", 894, 640, 16, Cyan);
}
void Renderer::menu(const Game& g) {
    if (g.screen == Screen::Menu) {
        text("A DSA ARCADE PROJECT", 82, 90, 22, Cyan);
        text("PARCEL", 77, 140, 86, White); text("PANIC", 77, 223, 86, Gold);
        text("CONVEYOR MAYHEM", 84, 311, 22, Cyan);
        button("PLAY / ENTER", 82, 340, 320, 56, true); button("LEVEL SELECT", 82, 410, 320, 50);
        button("HELP & CONTROLS", 82, 474, 320, 50); button("QUIT", 82, 538, 320, 50);
        box(520, 126, 675, 470);
        text("ROUTE. REACT. KEEP IT MOVING.", 555, 160, 27, White);
        text("One factory. Four structures. Total mayhem.", 555, 206, 21, Muted);
        DrawLineEx({560, 350}, {1150, 350}, 54, Track);
        DrawLineEx({860, 350}, {860, 270}, 50, Track);
        for (int i = 0; i < 7; ++i) {
            float x = 560 + std::fmod(g.clock * 70 + i * 93, 580.0f);
            box(x, 331, 38, 38, color(static_cast<Destination>(i % 3))); text(letter(static_cast<Destination>(i % 3)), x + 10, 339, 23, Ink);
        }
        DrawCircleV({860, 350}, 35, Ink); DrawCircleLines(860, 350, 35, Cyan); arrow({860, 350}, std::sin(g.clock) * 1.5f, 21, Cyan);
        text("Click junctions to match parcels to docks.", 555, 440, 22);
        text("Build combos. Rescue fragile freight.", 555, 478, 22, Muted);
        text("F4 skips levels instantly for your viva.", 555, 530, 22, Gold);
    } else if (g.screen == Screen::Select) {
        text("CHOOSE YOUR SHIFT", 70, 94, 43); text("Every layout is unlocked. Press 1-4 or select a card.", 70, 161, 23, Muted);
        for (int i = 0; i < 4; ++i) {
            LevelDefinition l = makeLevel(i); float x = 70.0f + i * 295;
            bool hover = CheckCollisionPointRec(GetMousePosition(), {x, 260, 275, 270}); box(x, 260, 275, 270, hover ? Track : Panel);
            text(TextFormat("0%i", i + 1), x + 20, 280, 48, Cyan); text(l.name, x + 20, 350, 23, Gold);
            text(TextFormat("%i parcels", l.tuning.total), x + 20, 400, 22); text(TextFormat("Quota %i / Score %i", l.tuning.quota, l.tuning.target), x + 20, 438, 18, Muted);
            text(i == 0 ? "Learn routing" : i == 1 ? "Merges + rush waves" : i == 2 ? "Fragile bypass + recycle" : "All types + tight loader", x + 20, 480, 17, Muted);
        }
        button("BACK / ESC", 70, 580, 220, 48);
    } else if (g.screen == Screen::Help) {
        text("THE FACTORY FIELD GUIDE", 70, 65, 40);
        const char* controls[10] = {"LEFT CLICK   switch a cyan junction", "RIGHT CLICK  emergency cell brake (cooldown)", "SPACE / ESC  pause and plan / resume", "Z            undo paused junction changes", "H            toggle the live structures HUD", "R / F5       restart the current level", "F1           circular queue demonstration", "F2           deadline min-heap demonstration", "F3           hash collision / probing demonstration", "F4 / SHIFT   next level / previous level"};
        for (int i = 0; i < 10; ++i) text(controls[i], 74, 145 + i * 39.0f, 21, i % 2 ? Muted : White);
        box(755, 138, 450, 420); text("MATCH THE LETTERS", 780, 163, 25, Cyan);
        text("R red   B blue   Y yellow", 780, 210, 21);
        text("X damaged -> green recycling", 780, 247, 21, color(Destination::Recycle));
        text("F fragile -> avoid crusher tiles", 780, 284, 21, Gold);
        text("! express -> shorter deadline", 780, 321, 21, Gold);
        text("Correct docks feed one shared loader.", 780, 373, 19, Muted);
        text("It ships the earliest deadline first.", 780, 407, 19, Muted);
        text("Reach the quota AND score target.", 780, 456, 21, Cyan);
        text("Wrong docks / expiry break combos.", 780, 492, 19, Muted);
        button("BACK / ESC", 70, 600, 220, 48);
    }
}
void Renderer::demonstration(const Game& g) {
    const Demos& d = g.demos;
    DrawRectangle(0, 7, Config::WindowWidth, Config::WindowHeight - 46, Fade(Ink, 0.92f));
    box(60, 80, 1160, 570);
    text(d.mode == 1 ? "F1 / CIRCULAR QUEUE" : d.mode == 2 ? "F2 / DEADLINE MIN-HEAP" : "F3 / OPEN-ADDRESS HASH TABLE", 90, 105, 32, Cyan);
    text("ENTER next operation   |   BACKSPACE reset   |   ESC close   |   Simulation is held", 90, 153, 20, Muted);
    if (d.mode == 1) {
        const CircularQueue& q = d.queue;
        text(TextFormat("size %i / %i     front %i     rear (next write) %i", q.size(), q.capacity(), q.front(), q.rear()), 90, 220, 25);
        for (int i = 0; i < q.capacity(); ++i) {
            float x = 100 + i * 174.0f; box(x, 300, 148, 110, q.slot(i) ? Track : Ink);
            text(TextFormat("slot %i", i), x + 30, 315, 22, Muted); text(q.slot(i) ? TextFormat("#%i", q.slot(i)) : "empty", x + 22, 357, 28, q.slot(i) ? Gold : Muted);
            if (i == q.front()) text("FRONT", x + 24, 430, 21, Cyan);
            if (i == q.rear()) text("REAR", x + 24, 462, 21, Gold);
        }
        text("Real fixed-capacity int[] - O(1) enqueue / dequeue, modulo wraparound", 90, 515, 22, Muted);
    } else if (d.mode == 2) {
        const MinHeap& heap = d.heap;
        text(TextFormat("size %i   allocated %i   priority = deadline, then arrival sequence", heap.size(), heap.capacity()), 90, 205, 23);
        for (int i = 0; i < heap.size(); ++i) {
            int depth = i < 1 ? 0 : i < 3 ? 1 : 2;
            int first = depth == 0 ? 0 : depth == 1 ? 1 : 3;
            int count = depth == 0 ? 1 : depth == 1 ? 2 : 4;
            float x = 115 + (i - first + 0.5f) * (600.0f / count); float y = 290 + depth * 95.0f;
            if (i > 0) {
                int parent = (i - 1) / 2, pd = parent == 0 ? 0 : 1, pf = pd == 0 ? 0 : 1, pc = pd == 0 ? 1 : 2;
                Vector2 p{115 + (parent - pf + 0.5f) * (600.0f / pc), 290 + pd * 95.0f}; DrawLineEx(p, {x, y}, 2, Track);
            }
            DrawCircleV({x, y}, 35, i == 0 ? Color{26, 105, 117, 255} : Track);
            text(TextFormat("#%i", heap.at(i).id), x - 23, y - 17, 19); text(TextFormat("d:%i", heap.at(i).deadline), x - 21, y + 6, 17, Gold);
        }
        text("ACTUAL HEAP ARRAY", 820, 266, 22, Cyan);
        for (int i = 0; i < heap.size(); ++i) text(TextFormat("[%i] #%i d:%i seq:%llu", i, heap.at(i).id, heap.at(i).deadline, heap.at(i).sequence), 820, 309 + i * 32.0f, 18, i == 0 ? Gold : White);
        text("Only parcels at a correct dock enter the gameplay heap.", 90, 535, 21, Muted);
    } else {
        const HashTable& t = d.table;
        text(TextFormat("hash(id) = id %% %i   |   records %i   |   linear probing and tombstones", t.capacity(), t.size()), 90, 205, 23);
        int visible = std::min(t.capacity(), 15);
        for (int i = 0; i < visible; ++i) {
            float x = 100 + (i / 5) * 365.0f, y = 260 + (i % 5) * 49.0f;
            const Bucket& b = t.bucket(i); box(x, y, 335, 41, b.state == BucketState::Occupied ? Track : Ink);
            text(TextFormat("[%02i]", i), x + 10, y + 10, 20, Muted);
            text(b.state == BucketState::Occupied ? TextFormat("#%i  home:%i", b.parcel.id, t.hash(b.parcel.id)) : b.state == BucketState::Tombstone ? "TOMBSTONE" : "EMPTY", x + 80, y + 10, 20, b.state == BucketState::Occupied ? Cyan : Gold);
        }
        text("IDs 1, 8, 15 deliberately collide at capacity 7. Searches read this actual table.", 90, 535, 20, Muted);
    }
    text(TextFormat("STEP %02i", d.step), 90, 588, 22, Gold); text(d.event.c_str(), 210, 590, 19, White);
}
void Renderer::draw(const Game& g) {
    background(g);
    if (g.screen == Screen::Playing || g.screen == Screen::Results) { factory(g); dashboard(g); }
    else menu(g);
    if (g.screen == Screen::Results && !g.demos.mode) {
        DrawRectangle(0, 7, Config::WindowWidth, Config::WindowHeight - 46, Fade(Ink, 0.80f)); box(350, 190, 580, 390);
        bool won = g.simulation.outcome == Outcome::Won;
        if (won) {
            for (int i = 0; i < 26; ++i) {
                float x = 368 + std::fmod(i * 67.0f, 540.0f);
                float y = 197 + std::fmod(g.clock * (28 + i % 4 * 9) + i * 43.0f, 365.0f);
                DrawRectanglePro({x, y, 5, 9}, {2, 4}, g.clock * 80 + i * 21.0f, Fade(i % 2 ? Cyan : Gold, 0.45f));
            }
        }
        text(won ? "SHIFT COMPLETE!" : "SHIFT OVER", 395, 223, 43, won ? Cyan : Gold);
        text(TextFormat("%s  /  SCORE %i", g.simulation.conveyor.layout.name, g.simulation.dispatch.score), 395, 293, 22);
        text(TextFormat("Delivered %i    Failed %i    Combo %ix", g.simulation.dispatch.delivered, g.simulation.dispatch.failed, g.simulation.dispatch.multiplier()), 395, 339, 21, Muted);
        text(won ? "Quota and score target achieved." : g.simulation.jam >= Config::JamLimit ? "Factory jammed. Try staggering the incoming lanes." : "Quota or score target missed. Plan your junctions!", 395, 387, 18, Gold);
        button("RETRY / F5", 390, 440, 230, 52); button("NEXT / F4", 650, 440, 230, 52, true); button("MAIN MENU", 520, 510, 230, 42);
    }
    if (g.demos.mode) demonstration(g);
}
