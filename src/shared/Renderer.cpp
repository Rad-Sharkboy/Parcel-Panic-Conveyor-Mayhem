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
    text("F1  Queue     F2  Heap     F3  Hash     H  DSA panel", 838, 691, 16, Muted);
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
    if (g.paused) {
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
    BeginScissorMode(22, 138, 828, 532);
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
    for (int i = 0; i < Config::Cells; ++i) {
        const Cell& cell = layout.cells[i];
        if (cell.kind == CellKind::Empty) {
            continue;
        }
        Vector2 p = center(i);
        if (cell.kind == CellKind::Dock) {
            dock(p, cell.dock);
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
        box({p.x - 19, p.y - 39, 38, 18}, Raised);
        centered(TextFormat("IN %i", i + 1), {p.x - 19, p.y - 40, 38, 18}, 12, Cyan, true);
        DrawLineEx({p.x - 19, p.y - 15}, {p.x - 19, p.y + 15}, 4, Cyan);
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
    if (g.titleTimer > 0) {
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
    box({875, 136, 365, 536}, Panel, 0.06f);
    text(g.hud ? "Live structures" : "Routing guide", 895, 155, 24);
    text(g.hud ? "H  Hide details     Q  Change input" : "H  Show DSA details", 895, 190, 16, Muted);
    int cooldown = conveyor.brakeReady - s.tick;
    if (cooldown < 0) {
        cooldown = 0;
    }
    DrawCircle(901, 231, 4, cooldown ? Gold : Cyan);
    text(cooldown ? TextFormat("Brake ready in %.1f s", cooldown * Config::TickSeconds)
                  : "Brake ready / right click a belt",
         915, 220, 17, cooldown ? Gold : Cyan);
    DrawLine(895, 252, 1219, 252, Border);
    if (g.hud) {
        const CircularQueue& queue = conveyor.inputs[g.inspectedLane];
        text(TextFormat("Input queue %i  / click to change", g.inspectedLane + 1), 895, 265, 16, White, true);
        text(TextFormat("%i / %i", queue.size(), queue.getCapacity()), 1172, 265, 18, Cyan);
        float slotWidth = 324.0f / queue.getCapacity();
        for (int i = 0; i < queue.getCapacity(); ++i) {
            Rectangle slot{895 + i * slotWidth, 297, slotWidth - 5, 31};
            box(slot, queue.getSlot(i) ? Raised : Background);
            centered(queue.getSlot(i) ? TextFormat("%i", queue.getSlot(i)) : "-", slot, 16,
                     queue.getSlot(i) ? White : Muted);
            if (i == queue.getFront()) {
                DrawRectangleRec({slot.x, slot.y + 30, slot.width, 2}, Cyan);
            }
        }
        text(TextFormat("Front %i   Rear %i   (last insert)", queue.getFront(), queue.getRear()), 895, 338,
             15, Muted);
        if (queue.isEmpty()) {
            text("Empty: no parcels waiting at this input.", 895, 355, 14, Muted);
        }
        HeapEntry root;
        bool hasRoot = s.dispatch.ready.peek(root);
        text("Dispatch heap", 895, 370, 18, White, true);
        text(TextFormat("%i ready", s.dispatch.ready.size()), 1142, 370, 17, Cyan);
        text(hasRoot ? TextFormat("Next #%i / deadline %i", root.id, root.deadline)
                     : "Empty: no dock parcels waiting to load.",
             895, 395, 15, Muted);
        text(TextFormat("Hash table   %i records / %i buckets", s.dispatch.registry.size(),
                        s.dispatch.registry.getCapacity()),
             895, 426, 16, Muted);
        text(conveyor.history.isEmpty()
                 ? "Undo stack  0 / pause, then click a switch"
                 : TextFormat("Undo stack   %i changes / Z to undo", conveyor.history.size()),
             895, 450, 15, Gold);
    } else {
        const char* names[4] = {"Red shipping", "Blue shipping", "Yellow shipping", "Recycling / damaged"};
        for (int i = 0; i < 4; ++i) {
            Destination destination = (Destination)(i);
            box({895, 271 + i * 34.0f, 25, 25}, destinationColor(destination));
            centered(destinationLetter(destination), {895, 271 + i * 34.0f, 25, 25}, 17, Background, true);
            text(names[i], 933, 273 + i * 34.0f, 17);
        }
        text("F  Fragile: take the safe bypass", 895, 417, 16, Muted);
        text("!   Express: reach the loader quickly", 895, 444, 16, Muted);
    }
    if (g.paused) {
        button("Resume", Resume, true);
        button("Restart", Restart);
        button("Main menu", MainMenu);
    } else {
        text("UP NEXT", 895, 482, 13, Muted, true);
        for (int i = 0; i < conveyor.layout.inputCount; ++i) {
            int id = 0;
            id = conveyor.inputs[i].peek();
            bool has = id != -1;
            const Parcel* p = has ? s.dispatch.registry.find(id) : nullptr;
            text(TextFormat("IN %i", i + 1), 895, 508 + i * 24.0f, 15, Muted);
            text(p ? TextFormat("#%i   %s  %s", id, destinationLetter(p->destination),
                                p->kind == ParcelKind::Fragile   ? "Fragile"
                                : p->kind == ParcelKind::Express ? "Express"
                                : p->kind == ParcelKind::Damaged ? "Recycle"
                                                                 : "Normal")
                   : "Lane clear",
                 946, 508 + i * 24.0f, 16, p ? destinationColor(p->destination) : White);
        }
    }
    DrawLine(895, 582, 1219, 582, Border);
    text(g.paused ? "Click switches, then Z to undo." : "Space pauses the belts for planning.", 895, 595, 16,
         g.paused ? Gold : White);
    text("F1 / F2 / F3  Step through the DSA demos", 895, 622, 15, Muted);
    text("F4  Next shift     Shift+F4  Previous", 895, 646, 15, Cyan);
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
    if (g.demos.mode) {
        demonstration(g);
    }
}
