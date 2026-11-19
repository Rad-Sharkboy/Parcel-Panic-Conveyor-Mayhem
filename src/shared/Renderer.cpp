#include "Renderer.h"
#include "Ui.h"
#include <cmath> // Angles, distances, rounding and looping motion for the Raylib graphics.

using namespace Ui;
namespace {
const Color Floor{25, 35, 45, 255};
const Color Belt{52, 67, 79, 255};
const Color Rail{96, 113, 121, 255};

void conveyorFrame(const LevelDefinition& layout) {
    // Draw connections first, then machines and parcels: no later tile covers a switch.
    // Complete each layer before starting the next, leaving continuous rails.
    for (int layer = 0; layer < 3; ++layer) {
        float width = layer == 2 ? 30.0f : 38.0f;
        Color tint = layer == 0 ? Fade(BLACK, 0.25f) : layer == 1 ? Rail : Belt;
        float offset = layer == 0 ? 5.0f : 0;
        for (int i = 0; i < Config::Cells; ++i) {
            const Cell& c = layout.cells[i];
            if (c.kind == CellKind::Empty) {
                continue;
            }
            Vector2 p = Renderer::center(i);
            p.y += offset;
            DrawCircleV(p, width / 2, tint);
            if (c.kind == CellKind::Dock) {
                continue;
            }
            int branches = c.kind == CellKind::Junction ? 2 : 1;
            for (int branch = 0; branch < branches; ++branch) {
                if (c.next[branch] < 0) {
                    continue;
                }
                Vector2 n = Renderer::center(c.next[branch]);
                n.y += offset;
                DrawLineEx(p, n, width, tint);
            }
        }
    }
}
void conveyorMotion(const Game& g) {
    const LevelDefinition& layout = g.simulation.conveyor.layout;
    for (int i = 0; i < Config::Cells; ++i) {
        const Cell& c = layout.cells[i];
        if (c.kind == CellKind::Empty || c.kind == CellKind::Dock) {
            continue;
        }
        Vector2 p = Renderer::center(i);
        int branches = c.kind == CellKind::Junction ? 2 : 1;
        for (int branch = 0; branch < branches; ++branch) {
            int target = c.next[branch];
            if (target < 0) {
                continue;
            }
            Vector2 n = Renderer::center(target);
            float dx = n.x - p.x, dy = n.y - p.y;
            float length = std::sqrt(dx * dx + dy * dy);
            dx /= length;
            dy /= length;
            bool active = branches == 1 || branch == c.choice;
            float phase = g.beltOffset[i][branch];
            for (float offset = std::fmod(phase, Config::BeltRibSpacing); offset < length;
                 offset += Config::BeltRibSpacing) {
                Vector2 q{p.x + dx * offset, p.y + dy * offset};
                DrawLineEx({q.x - dy * 12, q.y + dx * 12}, {q.x + dy * 12, q.y - dx * 12}, 2,
                           active ? Color{68, 84, 95, 255} : Color{57, 71, 82, 255});
            }
            // Fade the moving arrow at either end, avoiding a visible wrap jump.
            float offset = std::fmod(phase + length * 0.58f, length);
            float distanceToEnd = offset;
            if (length - offset < distanceToEnd) {
                distanceToEnd = length - offset;
            }
            float opacity = distanceToEnd / 8;
            if (opacity > 1) {
                opacity = 1;
            }
            arrow({p.x + dx * offset, p.y + dy * offset}, std::atan2(dy, dx), 6,
                  Fade(active ? Color{153, 171, 178, 255} : Border, opacity));
            if (branches == 2 && active) {
                DrawLineEx({p.x + dx * 19, p.y + dy * 19}, {p.x + dx * 28, p.y + dy * 28}, 4, Cyan);
            }
        }
    }
}
void dock(Vector2 p, Destination destination) {
    Color tint = destinationColor(destination);
    box({p.x - 23, p.y - 22, 46, 48}, Background);
    box({p.x - 21, p.y - 22, 42, 42}, tint);
    box({p.x - 16, p.y - 16, 32, 31}, Panel);
    for (int j = 0; j < 3; ++j) {
        DrawLineEx({p.x - 13, p.y - 11 + j * 9.0f}, {p.x + 13, p.y - 11 + j * 9.0f}, 2, Border);
    }
    box({p.x - 12, p.y - 13, 24, 25}, tint);
    centered(destinationLetter(destination), {p.x - 12, p.y - 14, 24, 26}, 20, Background, true);
    DrawCircleV({p.x - 12, p.y + 23}, 2, tint);
    DrawCircleV({p.x + 12, p.y + 23}, 2, tint);
}
void hazard(Vector2 p, float clock) {
    box({p.x - 22, p.y - 22, 44, 44}, Color{132, 80, 55, 255});
    box({p.x - 17, p.y - 17, 34, 34}, Background);
    for (int i = -16; i <= 12; i += 8) {
        DrawLineEx({p.x + i, p.y - 22}, {p.x + i + 5, p.y - 16}, 4, Gold);
        DrawLineEx({p.x + i, p.y + 16}, {p.x + i + 5, p.y + 22}, 4, Gold);
    }
    float close = 2 + (1 + std::sin(clock * 4)) * 2;
    DrawRectangleRec({p.x - 15, p.y - 15, 30, close}, Rail);
    DrawRectangleRec({p.x - 15, p.y + 15 - close, 30, close}, Rail);
    centered("!", {p.x - 10, p.y - 13, 20, 24}, 20, Gold, true);
}
void routePreview(const Game& g) {
    int index = g.hoverCell;
    if (index < 0 || g.inspector.open || g.briefing || g.demos.mode) { return; }
    const LevelDefinition& layout = g.simulation.conveyor.layout;
    const Cell& junction = layout.cells[index];
    if (junction.kind != CellKind::Junction) { return; }
    // Walk each branch only as far as the next decision or dock. No pathfinding.
    for (int branch = 0; branch < 2; branch++) {
        Color tint = branch == junction.choice ? Cyan : Gold;
        int current = index, next = junction.next[branch];
        bool hazardOnRoute = false;
        for (int steps = 0; steps < Config::Cells && next >= 0 && next < Config::Cells; steps++) {
            DrawLineEx(Renderer::center(current), Renderer::center(next), 6, Fade(tint, 0.42f));
            const Cell& cell = layout.cells[next];
            if (cell.kind == CellKind::Hazard) { hazardOnRoute = true; }
            if (cell.kind == CellKind::Junction || cell.kind == CellKind::Dock || cell.next[0] < 0) {
                Vector2 p = Renderer::center(next);
                const char* label = cell.kind == CellKind::Dock ? TextFormat("%s DOCK", destinationLetter(cell.dock)) :
                                    hazardOnRoute ? "CRUSHER ROUTE" : "NEXT SWITCH";
                float width = textWidth(label, 13, true) + 12;
                float x = p.x - width / 2;
                if (x < 28) { x = 28; }
                if (x + width > 838) { x = 838 - width; }
                float labelY = p.y - 43 - branch * 23.0f;
                box({x, labelY, width, 20}, Background);
                centered(label, {x, labelY, width, 20}, 13, tint, true);
                break;
            }
            current = next;
            next = cell.next[0];
        }
    }
}
} // namespace
Vector2 Renderer::center(int cell) {
    return {(float)(Config::BoardX + (cell % Config::Columns) * Config::Tile + Config::Tile / 2),
            (float)(Config::BoardY + (cell / Config::Columns) * Config::Tile + Config::Tile / 2)};
}
void Renderer::background(const Game& g) {
    ClearBackground(Background);
    DrawRectangle(0, 0, Config::WindowWidth, 3, Cyan);
    DrawRectangle(0, 682, Config::WindowWidth, 38, Panel);
    text("F4  Next level     Shift+F4  Previous     F5  Restart", 28, 691, 17, White);
    const char* labels[3] = {"F1 Queue", "F2 Heap", "F3 Hash"};
    for (int i = 0; i < 3; i++) {
        bool hover = CheckCollisionPointRec(GetMousePosition(), DemoTabs[i]);
        box(DemoTabs[i], hover || g.demos.mode == i + 1 ? Border : Raised);
        centered(labels[i], DemoTabs[i], 16, White);
    }
    box(InfoToggle, CheckCollisionPointRec(GetMousePosition(), InfoToggle) ? Border : Raised);
    centered("H Help / info", InfoToggle, 16, Cyan);
    box(InspectorToggle, g.inspector.open ? Cyan : Raised);
    centered("I  Live inspector", InspectorToggle, 16, g.inspector.open ? Background : Cyan);
    if (g.screen != Screen::Playing && g.screen != Screen::Results) {
        return;
    }
    const Simulation& s = g.simulation;
    const Config::LevelTuning& tuning = s.conveyor.layout.tuning;
    box({28, 23, 37, 37}, Cyan);
    centered(TextFormat("%02i", s.level + 1), {28, 23, 37, 37}, 20, Background, true);
    text(s.conveyor.layout.name, 78, 20, 32);
    text("Parcel Panic / Conveyor Mayhem", 79, 58, 16, Muted);
    text("DELIVERIES", 446, 23, 15, Muted);
    text(TextFormat("%i / %i", s.dispatch.delivered, tuning.quota), 446, 40, 36);
    meter({446, 84, 118, 5}, (float)(s.dispatch.delivered) / tuning.quota, Cyan);
    text("SCORE", 608, 23, 15, Muted);
    text(TextFormat("%05i", s.dispatch.score), 608, 40, 36, Gold);
    text(TextFormat("Target %i", tuning.target), 738, 58, 16, Muted);
    text("COMBO", 888, 23, 15, Muted);
    text(TextFormat("%ix", s.dispatch.multiplier()), 888, 40, 36, Cyan);
    text("BELT CONGESTION", 1020, 23, 15, Muted);
    text(TextFormat("%i%%", s.jam), 1183, 53, 20, s.jam > 60 ? destinationColor(Destination::Red) : White);
    meter({1020, 61, 146, 9}, (float)(s.jam) / Config::JamLimit,
          s.jam > 60 ? destinationColor(Destination::Red) : Cyan);
    DrawLine(28, 101, 1240, 101, Border);
    if (g.feedbackTime > 0) {
        text(g.feedback, 29, 110, 16, Gold, true);
    } else if (g.paused) {
        text("PAUSED", 29, 110, 16, Gold, true);
        text("Plan your junctions. Z undoes the last switch. Space resumes.", 120, 110, 16, White);
    } else if (s.rush() || s.rushWarning()) {
        DrawCircle(35, 119, 4, Gold);
        text(s.rush() ? "Rush wave active - keep the lanes clear!"
                      : "Rush wave incoming - prepare your routes!",
             48, 110, 16, Gold, true);
    } else {
        text("Left click  Switch junction     Right click  Brake     Space  Pause & plan", 29, 110, 16,
             Muted);
    }
    text(TextFormat("Arrivals  %i / %i", s.generated, tuning.total), 1080, 110, 16, Muted);
}
void Renderer::factory(const Game& g) {
    const Simulation& s = g.simulation;
    const LevelDefinition& layout = s.conveyor.layout;
    box({20, 136, 832, 536}, Border, 0.06f);
    box({21, 137, 830, 534}, Floor, 0.06f);
    clip({22, 138, 828, 532});
    for (int x = Config::BoardX; x <= Config::BoardX + Config::Columns * Config::Tile; x += Config::Tile) {
        DrawLine(x, 140, x, 668, Color{30, 42, 53, 255});
    }
    for (int y = Config::BoardY; y <= Config::BoardY + Config::Rows * Config::Tile; y += Config::Tile) {
        DrawLine(28, y, 844, y, Color{30, 42, 53, 255});
    }
    text("SORTING FLOOR", 43, 151, 13, Muted, true);
    text(TextFormat("SHIFT %02i  /  %.1f s", s.level + 1, s.tick * Config::TickSeconds), 672, 151, 13, Muted);
    conveyorFrame(layout);
    conveyorMotion(g);
    routePreview(g);
    for (int i = 0; i < Config::Cells; ++i) {
        const Cell& cell = layout.cells[i];
        if (cell.kind == CellKind::Empty) {
            continue;
        }
        Vector2 p = center(i);
        if (cell.kind == CellKind::Dock) {
            dock(p, cell.dock);
            text(cell.dock == Destination::Recycle ? "RECYCLE" : "DOCK", p.x - 21, p.y + 30, 11,
                 destinationColor(cell.dock), true);
        } else if (cell.kind == CellKind::Hazard) {
            hazard(p, g.beltClock);
        } else if (cell.kind == CellKind::Junction) {
            DrawCircleV({p.x, p.y + 3}, 21, Fade(BLACK, 0.3f));
            DrawCircleV(p, 21, Cyan);
            DrawCircleV(p, 18, Panel);
            if (g.junctionPulse[i] > 0) {
                DrawCircleLinesV(p, 22 + (1 - g.junctionPulse[i]) * 7, Fade(Cyan, g.junctionPulse[i]));
            }
            arrow(p, g.junctionAngle[i], 10, Cyan);
        }
        if (cell.brakeUntil > s.tick) {
            DrawRectangleLinesEx({p.x - 21, p.y - 21, 42, 42}, 2, Gold);
            box({p.x - 10, p.y - 35, 20, 16}, Gold);
            centered("II", {p.x - 10, p.y - 36, 20, 16}, 13, Background, true);
        }
        if (i == g.hoverCell && !g.demos.mode && g.screen == Screen::Playing) {
            if (cell.kind == CellKind::Junction) {
                DrawCircleLinesV(p, 25, White);
            } else if (cell.kind != CellKind::Dock) {
                DrawRectangleLinesEx({p.x - 22, p.y - 22, 44, 44}, 1, Fade(White, 0.65f));
            }
        }
    }
    for (int i = 0; i < layout.inputCount; ++i) {
        Vector2 p = center(layout.inputs[i]);
        const CircularQueue& waiting = s.conveyor.inputs[i];
        box({p.x - 19, p.y - 39, 38, 18}, Raised);
        centered(TextFormat("IN %i", i + 1), {p.x - 19, p.y - 40, 38, 18}, 12, Cyan, true);
        DrawLineEx({p.x - 19, p.y - 15}, {p.x - 19, p.y + 15}, 4, Cyan);
        box({p.x + 24, p.y - 40, 91, 19}, Background);
        for (int item = 0; item < waiting.size() && item < 3; item++) {
            int id = waiting.getSlot((waiting.getFront() + item) % waiting.getCapacity());
            const Parcel* queued = s.dispatch.registry.find(id);
            Rectangle slot{p.x + 27 + item * 28.0f, p.y - 38, 24, 15};
            box(slot, queued ? destinationColor(queued->destination) : Border);
            centered(TextFormat("%i", id), slot, 11, Background, true);
        }
        if (waiting.isEmpty()) { text("staging clear", p.x + 30, p.y - 37, 11, Muted); }
        if (waiting.isFull()) { text("FULL", p.x + 121, p.y - 37, 13, Gold, true); }
    }
    for (int i = 0; i < s.dispatch.registry.getCapacity(); ++i) {
        const Bucket& bucket = s.dispatch.registry.getBucket(i);
        if (bucket.state != BucketState::Occupied) {
            continue;
        }
        const Parcel& p = bucket.parcel;
        const ParcelMotion& m = g.motion[p.id];
        if (m.opacity <= 0) {
            continue;
        }
        parcel(m.position, p.destination, p.kind, m.opacity);
        if (p.id == g.inspectedParcel && p.status == ParcelStatus::OnConveyor) {
            DrawRectangleLinesEx({m.position.x - 19, m.position.y - 19, 38, 38}, 2,
                                 destinationColor(p.destination));
        }
        if (p.status == ParcelStatus::OnConveyor) {
            int lifetime =
                p.kind == ParcelKind::Express ? Config::ExpressDeadlineTicks : Config::DeadlineTicks;
            float ratio = (float)(p.deadline - s.tick) / lifetime;
            meter({m.position.x - 14, m.position.y + 20, 28, 3}, ratio,
                  Fade(p.deadline - s.tick < 25 ? destinationColor(Destination::Red)
                                                : destinationColor(p.destination),
                       m.opacity));
        }
    }
    for (int i = 0; i < Config::ParticleCount; ++i) {
        const Particle& p = g.particles[i];
        if (p.life > 0) {
            DrawRectangleRec({p.position.x - 2, p.position.y - 2, 4, 4},
                             Fade(p.color, p.life / Config::ParticleSeconds));
        }
    }
    for (int i = 0; i < Config::PopupCount; ++i) {
        const ScorePopup& p = g.popups[i];
        if (p.life <= 0) {
            continue;
        }
        float alpha = p.life * 3;
        if (alpha > 1) {
            alpha = 1;
        }
        const char* label = TextFormat("%+i", p.points);
        float width = textWidth(label, 20, true) + 14;
        float x = p.position.x - width / 2;
        if (x < 26) {
            x = 26;
        }
        if (x > 842 - width) {
            x = 842 - width;
        }
        float y = p.position.y - 48;
        if (y < 174) {
            y = 174;
        }
        box({x, y, width, 27}, Fade(Background, alpha * 0.9f));
        centered(label, {x, y, width, 27}, 20,
                 Fade(p.points > 0 ? Cyan : destinationColor(Destination::Red), alpha), true);
    }
    if (g.comboTimer > 0) {
        float width = 214, y = 603 + (1 - g.comboTimer / Config::PopupSeconds) * 6;
        box({329, y, width, 42}, Background);
        centered(TextFormat("%ix delivery combo!", g.comboShown), {329, y, width, 42}, 22, Gold);
    }
    if (g.titleTimer > 0 && !g.briefing) {
        float alpha = g.titleTimer * 2;
        if (alpha > 1) {
            alpha = 1;
        }
        box({250, 182, 370, 49}, Fade(Background, alpha * 0.96f));
        centered(TextFormat("Shift %02i  /  %s", s.level + 1, layout.name), {250, 182, 370, 49}, 22,
                 Fade(White, alpha));
    }
    EndScissorMode();
}
void Renderer::dashboard(const Game& g) {
    const Simulation& s = g.simulation;
    const ConveyorSystem& conveyor = s.conveyor;
    bool inspecting = g.inspectedParcel != 0 && !g.demos.mode && !g.showHelp;
    box({875, 136, 365, 536}, Panel, 0.06f);
    text(inspecting ? "Parcel inspection" : g.showHelp ? "Factory guide" : "Shared loading bay", 895, 154, 25);
    text(inspecting ? "Move away to return to the loading bay" :
         g.showHelp ? "H  Return to loading bay" : "All docks share one automatic loader", 895, 188, 16, Muted);
    // These are alternative views of the same space, never overlapping layers.
    if (inspecting) {
        parcelInfo(g);
    } else if (g.showHelp) {
        const char* names[4] = {"Red shipping", "Blue shipping", "Yellow shipping", "Recycling / damaged"};
        for (int i = 0; i < 4; i++) {
            box({895, 229 + i * 34.0f, 25, 25}, destinationColor((Destination)i));
            centered(destinationLetter((Destination)i), {895, 229 + i * 34.0f, 25, 25}, 17, Background, true);
            text(names[i], 933, 231 + i * 34.0f, 17);
        }
        text("F  Fragile: avoid the crusher", 895, 378, 17, Gold);
        text("!  Express: shorter delivery deadline", 895, 408, 16, Gold);
        text("Hover a switch to preview its routes.", 895, 443, 16, Muted);
        text("Cyan: selected. Gold: alternative.", 895, 466, 16, Muted);
        if (!g.paused) {
            text("Right click an input to stagger traffic.", 895, 494, 16, Muted);
            text("I  Inspect live structures and step time.", 895, 522, 16, Cyan);
        }
    } else {
        HeapEntry root;
        bool hasRoot = s.dispatch.ready.peek(root);
        const Parcel* next = hasRoot ? s.dispatch.registry.find(root.id) : nullptr;
        int remaining = s.dispatch.ticksUntilLoad(s.tick);
        text(next && next->status == ParcelStatus::Ready ? TextFormat("NEXT  #%i   /   %s dock", next->id,
             destinationLetter(next->destination)) : hasRoot ? "Clearing expired entries" : "Waiting for dock arrivals",
             895, 220, 21, next ? destinationColor(next->destination) : White, true);
        text(remaining ? TextFormat("Next loading slot in %.1f s", remaining * Config::TickSeconds) :
                         "Loader ready for the next shipment", 895, 249, 16, Muted);
        meter({895, 273, 324, 4}, 1.0f - (float)remaining / conveyor.layout.tuning.loaderTicks, Cyan);
        int waitingCount = 0;
        for (int i = 0; i < s.dispatch.ready.size(); i++) {
            const HeapEntry& entry = s.dispatch.ready.getEntry(i);
            const Parcel* p = s.dispatch.registry.find(entry.id);
            if (p && p->status == ParcelStatus::Ready) { waitingCount++; }
            if (i >= 5) { continue; }
            Rectangle tile = loadingSlot(i);
            bool valid = p && p->status == ParcelStatus::Ready;
            box(tile, valid ? destinationColor(p->destination) : Border);
            centered(TextFormat("#%i", entry.id), {tile.x, tile.y, tile.width, 25}, 18, Background, true);
            centered(valid ? TextFormat("%.1fs", (entry.deadline - s.tick) * Config::TickSeconds) : "expired",
                     {tile.x, tile.y + 24, tile.width, 21}, 13, Background);
            if (i == 0) { DrawRectangleLinesEx(tile, 2, White); }
        }
        text(TextFormat("%i waiting   /   %i shipped", waitingCount, s.dispatch.delivered), 895, 335, 17, Cyan);
        text(TextFormat("Input %i  /  Q to change", g.inspectedLane + 1), 895, 368, 19, White, true);
        const CircularQueue& queue = conveyor.inputs[g.inspectedLane];
        text(TextFormat("%i / %i", queue.size(), queue.getCapacity()), 1154, 370, 17, queue.isFull() ? Gold : Cyan);
        for (int i = 0; i < 6; i++) {
            Rectangle tile = waitingSlot(i);
            int id = i < queue.size() ? queue.getSlot((queue.getFront() + i) % queue.getCapacity()) : 0;
            const Parcel* p = s.dispatch.registry.find(id);
            box(tile, p ? destinationColor(p->destination) : Background);
            centered(id ? TextFormat("#%i", id) : "-", tile, 18, p ? Background : Muted, true);
            if (i == 0) { DrawRectangleLinesEx(tile, 2, Cyan); }
        }
        bool blocked = conveyor.layout.cells[conveyor.layout.inputs[g.inspectedLane]].parcel != 0 ||
                       conveyor.layout.cells[conveyor.layout.inputs[g.inspectedLane]].brakeUntil > s.tick;
        text(queue.isFull() ? "FULL / arrivals waiting for space" : queue.size() > 0 && blocked ?
             "FRONT waits: entry belt is blocked" : "FRONT enters as soon as the belt is free", 895, 443, 15, queue.isFull() ? Gold : Muted);
        const Parcel* last = s.dispatch.registry.find(g.lastEntered[g.inspectedLane]);
        text(last ? TextFormat("Last to belt: #%i / %s", last->id, destinationLetter(last->destination)) :
                    "Last to belt: waiting for first parcel", 895, 464, 16, last ? destinationColor(last->destination) : Muted);
        if (!g.paused) {
            const Parcel* event = s.dispatch.registry.find(s.dispatch.lastEventId);
            text("LAST SHIPMENT", 895, 501, 13, Muted, true);
            const char* reason = event && event->status == ParcelStatus::Rejected ? "wrong dock" :
                                 event && event->status == ParcelStatus::Broken ? "fragile hit crusher" :
                                 event && event->status == ParcelStatus::Expired ? "deadline missed" : "delivered";
            text(event ? TextFormat("#%i  %s  (%+i)", event->id, reason, event->pointsAwarded) :
                         "Route a parcel to its matching dock.", 895, 525, 17, event && event->pointsAwarded < 0 ? Gold : Cyan);
        }
    }
    if (g.paused && !g.briefing) {
        button("Resume", Resume, true);
        button("Restart", Restart);
        button("Main menu", MainMenu);
        SwitchCommand top = conveyor.history.commandAt(0);
        if (top.cell >= 0) {
            text(TextFormat("Z restores cell %i / %i changes", top.cell, conveyor.history.size()), 895, 574, 16, Gold);
        } else { text("Click a switch to save a planning change.", 895, 574, 15, Gold); }
    }
    int cooldown = conveyor.brakeReady - s.tick;
    text(cooldown > 0 ? TextFormat("Brake recharges in %.1f s", cooldown * Config::TickSeconds) :
                       "Brake ready / right click a belt", 895, 599, 16, cooldown > 0 ? Gold : Cyan);
    button(g.paused ? "Space  /  Resume shift" : "Space  /  Pause and plan", PauseToggle);
}
void Renderer::parcelInfo(const Game& g) {
    const Parcel* parcel = g.simulation.dispatch.registry.find(g.inspectedParcel);
    if (!parcel) {
        return;
    }
    // A fixed sidebar card keeps the conveyor and its docks visible.
    const Rectangle& card = Ui::ParcelInfo;
    Color tint = destinationColor(parcel->destination);
    box({card.x + 3, card.y + 5, card.width, card.height}, Fade(BLACK, 0.35f), 0.08f);
    box(card, Panel, 0.08f);
    DrawRectangleRec({card.x, card.y + 12, 4, card.height - 24}, tint);
    text("PARCEL INFO", card.x + 18, card.y + 12, 13, Muted, true);
    text(TextFormat("Parcel #%i", parcel->id), card.x + 18, card.y + 33, 28);
    box({card.x + 247, card.y + 18, 42, 42}, tint);
    centered(destinationLetter(parcel->destination), {card.x + 247, card.y + 18, 42, 42}, 26, Background,
             true);
    const char* destinations[4] = {"Red dock", "Blue dock", "Yellow dock", "Recycling"};
    const char* kinds[4] = {"Normal", "Express", "Fragile", "Damaged"};
    const char* statuses[9] = {
        "Created",   "Waiting at input", "On conveyor", "At dock", "Waiting for loader",
        "Delivered", "Wrong dock",       "Crushed",     "Expired"};
    text(TextFormat("%s  /  %s", destinations[(int)parcel->destination], kinds[(int)parcel->kind]),
         card.x + 18, card.y + 71, 18, tint);
    DrawLine((int)card.x + 18, (int)card.y + 101, (int)card.x + 289, (int)card.y + 101, Border);
    text(statuses[(int)parcel->status], card.x + 18, card.y + 111, 17);
    if (terminal(parcel->status)) {
        text(TextFormat("%+i points", parcel->pointsAwarded), card.x + 196, card.y + 111, 17, Gold);
    } else {
        int remaining = parcel->deadline - g.simulation.tick;
        if (remaining < 0) {
            remaining = 0;
        }
        text(TextFormat("%.1f s left", remaining * Config::TickSeconds), card.x + 196, card.y + 111, 17,
             remaining < 25 ? Gold : Cyan);
        int lifetime =
            parcel->kind == ParcelKind::Express ? Config::ExpressDeadlineTicks : Config::DeadlineTicks;
        meter({card.x + 18, card.y + 139, 271, 4}, (float)remaining / lifetime, tint);
    }
    text(TextFormat("Cell %i  /  deadline tick %i", parcel->cell, parcel->deadline),
         card.x + 18, card.y + 153, 16, Muted);
    text("I: live record / move away to close", card.x + 18, card.y + 176, 15, Muted);
}
void Renderer::draw(const Game& g) {
    background(g);
    if (g.screen == Screen::Playing || g.screen == Screen::Results) {
        factory(g);
        dashboard(g);
    } else {
        menu(g);
    }
    if (g.screen == Screen::Results && !g.demos.mode) {
        results(g);
    }
    if (g.briefing && g.screen == Screen::Playing && !g.demos.mode) {
        DrawRectangle(22, 138, 828, 532, Fade(Background, 0.82f));
        box({155, 238, 575, 310}, Panel, 0.08f);
        text(TextFormat("SHIFT %02i  /  BRIEFING", g.simulation.level + 1), 187, 260, 16, Cyan, true);
        text(g.simulation.conveyor.layout.name, 185, 292, 35);
        const char* lessons[4][3] = {
            {"Match R and B parcels to their colored docks.", "Left click cyan switches. Hover to preview both routes.", "Space pauses to plan; Z undoes your planning switches."},
            {"Three inputs now share the central sorting belt.", "Turn red upward, blue downward, yellow straight ahead.", "Watch rush warnings. Right click an input to stagger traffic."},
            {"F parcels keep their color, but must avoid the crusher.", "Use the upper bypass first, then choose the colored dock.", "Express (!) and damaged (X) parcels arrive later in this shift."},
            {"Follow the vertical sorter: red, then blue, then yellow.", "X parcels continue to recycling. Keep F parcels off crushers.", "The loader is shared: urgent deadlines ship first."}};
        for (int i = 0; i < 3; i++) { text(lessons[g.simulation.level][i], 187, 351 + i * 30.0f, 17, i == 0 ? White : Muted); }
        button("Start shift / Enter   (skip briefing)", BriefingStart, true);
        text("Goal: reach BOTH the delivery quota and score target.", 187, 520, 16, Gold);
    }
    if (g.inspector.open && !g.demos.mode) { g.inspector.draw(g); }
    if (g.demos.mode) {
        demonstration(g);
    }
}
