#include "Game.h"
#include "Renderer.h"
#include "Ui.h"
#include <cmath> // Angles, distances, rounding and looping motion for the Raylib graphics.

Game::Game() {
    simulation.load(0);
}
void Game::start(int level) {
    simulation.load(level);
    selectedLevel = simulation.level;
    inspectedLane = 0;
    inspectedParcel = 0;
    parcelInfoTime = 0;
    for (int i = 0; i < Config::MaxInputs; i++) {
        lastEntered[i] = 0;
    }
    screen = Screen::Playing;
    paused = false;
    demos.mode = 0;
    accumulator = beltClock = comboTimer = 0;
    titleTimer = Config::LevelTitleSeconds;
    comboShown = 1;
    hoverCell = -1;
    for (int i = 0; i < Config::Cells; ++i) {
        junctionPulse[i] = 0;
        beltOffset[i][0] = beltOffset[i][1] = 0;
        junctionChoice[i] = simulation.conveyor.layout.cells[i].choice;
        const Cell& cell = simulation.conveyor.layout.cells[i];
        if (cell.kind != CellKind::Junction) {
            continue;
        }
        Vector2 p = Renderer::center(i), next = Renderer::center(cell.next[cell.choice]);
        junctionAngle[i] = std::atan2(next.y - p.y, next.x - p.x);
    }
    for (int i = 0; i <= Config::MaxParcels; ++i) {
        motion[i] = ParcelMotion{};
    }
    for (int i = 0; i < Config::PopupCount; ++i) {
        popups[i] = ScorePopup{};
    }
    for (int i = 0; i < Config::ParticleCount; ++i) {
        particles[i] = Particle{};
    }
}
void Game::input() {
    bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    if (IsKeyPressed(KEY_F4)) {
        start(simulation.level + (shift ? -1 : 1));
        return;
    }
    if (IsKeyPressed(KEY_F5) ||
        (IsKeyPressed(KEY_R) && (screen == Screen::Playing || screen == Screen::Results))) {
        start(simulation.level);
        return;
    }
    for (int i = 1; i <= 3; ++i) {
        if (IsKeyPressed(KEY_F1 + i - 1) || Ui::clicked(Ui::DemoTabs[i - 1])) {
            if (demos.mode == i) {
                demos.mode = 0;
            } else {
                demos.open(i);
            }
            return;
        }
    }
    if (demos.mode) {
        if (IsKeyPressed(KEY_ESCAPE) || Ui::clicked(Ui::DemoClose)) {
            demos.mode = 0;
        } else if (IsKeyPressed(KEY_BACKSPACE) || Ui::clicked(Ui::DemoReset)) {
            demos.open(demos.mode);
        } else if (IsKeyPressed(KEY_ENTER) || Ui::clicked(Ui::DemoNext)) {
            demos.advance();
        }
        return;
    }
    if (IsKeyPressed(KEY_H) || Ui::clicked(Ui::InfoToggle)) {
        if (screen == Screen::Menu || screen == Screen::Help) {
            screen = screen == Screen::Menu ? Screen::Help : Screen::Menu;
        } else {
            showHelp = !showHelp;
        }
        return;
    }
    if (screen == Screen::Menu) {
        if (IsKeyPressed(KEY_ENTER) || Ui::clicked(Ui::Play)) {
            start(selectedLevel);
        } else if (Ui::clicked(Ui::Select)) {
            screen = Screen::Select;
        } else if (Ui::clicked(Ui::Help)) {
            screen = Screen::Help;
        } else if (Ui::clicked(Ui::Quit) || IsKeyPressed(KEY_ESCAPE)) {
            quitting = true;
        }
        return;
    }
    if (screen == Screen::Select) {
        for (int i = 0; i < Config::Levels; ++i) {
            if (Ui::clicked(Ui::levelCard(i)) || IsKeyPressed(KEY_ONE + i)) {
                start(i);
                return;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE) || Ui::clicked(Ui::Back)) {
            screen = Screen::Menu;
        }
        return;
    }
    if (screen == Screen::Help) {
        if (IsKeyPressed(KEY_ESCAPE) || Ui::clicked(Ui::Back)) {
            screen = Screen::Menu;
        }
        return;
    }
    if (screen == Screen::Results) {
        if (Ui::clicked(Ui::Retry)) {
            start(simulation.level);
        } else if (Ui::clicked(Ui::Next) || IsKeyPressed(KEY_ENTER)) {
            start(simulation.level + 1);
        } else if (Ui::clicked(Ui::ResultsMenu) || IsKeyPressed(KEY_ESCAPE)) {
            screen = Screen::Menu;
        }
        return;
    }
    if (!showHelp && (IsKeyPressed(KEY_Q) || (inspectedParcel == 0 && Ui::clicked(Ui::QueueLane)))) {
        inspectedLane = (inspectedLane + 1) % simulation.conveyor.layout.inputCount;
    }
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) {
        paused = !paused;
    }
    if (paused && IsKeyPressed(KEY_Z)) {
        simulation.conveyor.undo();
    }
    if (paused) {
        if (Ui::clicked(Ui::Resume)) {
            paused = false;
        } else if (Ui::clicked(Ui::Restart)) {
            start(simulation.level);
            return;
        } else if (Ui::clicked(Ui::MainMenu)) {
            screen = Screen::Menu;
            return;
        }
    }
    // The info card is a UI surface; clicking it must not switch a belt behind it.
    if (inspectedParcel != 0 && CheckCollisionPointRec(GetMousePosition(), Ui::ParcelInfo)) {
        hoverCell = -1;
        return;
    }
    Vector2 mouse = GetMousePosition();
    int x = (int)((mouse.x - Config::BoardX) / Config::Tile);
    int y = (int)((mouse.y - Config::BoardY) / Config::Tile);
    hoverCell = mouse.x >= Config::BoardX && mouse.y >= Config::BoardY && x >= 0 && x < Config::Columns &&
                        y >= 0 && y < Config::Rows
                    ? cellAt(x, y)
                    : -1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        simulation.conveyor.switchJunction(hoverCell, paused);
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        simulation.conveyor.brake(hoverCell, simulation.tick);
    }
}
void Game::inspectParcel(float dt) {
    if (screen != Screen::Playing || demos.mode) {
        inspectedParcel = 0;
        parcelInfoTime = 0;
        return;
    }
    Vector2 mouse = GetMousePosition();
    if (inspectedParcel != 0 && CheckCollisionPointRec(mouse, Ui::ParcelInfo)) {
        parcelInfoTime = 3; // Keep the card readable while the pointer is over it.
        return;
    }
    parcelInfoTime -= dt;
    if (parcelInfoTime <= 0) {
        inspectedParcel = 0;
    }
    for (int id = 1; id <= simulation.generated; id++) {
        const ParcelMotion& visual = motion[id];
        if (visual.status != ParcelStatus::OnConveyor || visual.opacity < 0.25f) {
            continue;
        }
        // Use the drawn position, not the next logical cell, while a parcel is moving.
        Rectangle bounds{visual.position.x - 18, visual.position.y - 24, 36, 44};
        if (CheckCollisionPointRec(mouse, bounds)) {
            inspectedParcel = id;
            parcelInfoTime = 3;
            break;
        }
    }
}
void Game::turnJunctions(float dt) {
    if (screen != Screen::Playing || demos.mode) {
        return;
    }
    const LevelDefinition& layout = simulation.conveyor.layout;
    for (int j = 0; j < layout.junctionCount; ++j) {
        int i = layout.junctions[j];
        const Cell& cell = layout.cells[i];
        if (junctionChoice[i] != cell.choice) {
            junctionChoice[i] = cell.choice;
            junctionPulse[i] = 1;
        }
        Vector2 p = Renderer::center(i), next = Renderer::center(cell.next[cell.choice]);
        float target = std::atan2(next.y - p.y, next.x - p.x);
        float difference = target - junctionAngle[i];
        while (difference > PI) {
            difference -= 2 * PI;
        }
        while (difference < -PI) {
            difference += 2 * PI;
        }
        // Start from the visible angle, including rapid clicks and stack undo.
        float step = Config::JunctionTurnSpeed * dt;
        if (difference > step) {
            difference = step;
        }
        if (difference < -step) {
            difference = -step;
        }
        junctionAngle[i] += difference;
        junctionPulse[i] -= dt * 4;
        if (junctionPulse[i] < 0) {
            junctionPulse[i] = 0;
        }
    }
}
void Game::deliveryEffect(const Parcel& parcel) {
    int cell = parcel.eventCell;
    if (cell < 0) {
        cell = simulation.conveyor.layout.inputs[(parcel.id - 1) % simulation.conveyor.layout.inputCount];
    }
    Vector2 origin = Renderer::center(cell);
    if (parcel.status == ParcelStatus::Expired && motion[parcel.id].targetCell >= 0) {
        origin = motion[parcel.id].position;
    }
    for (int i = 0; i < Config::PopupCount; ++i) {
        if (popups[i].life > 0) {
            continue;
        }
        popups[i] = {origin, Config::PopupSeconds, parcel.pointsAwarded};
        break;
    }
    int emitted = 0;
    for (int i = 0; i < Config::ParticleCount && emitted < 10; ++i) {
        if (particles[i].life > 0) {
            continue;
        }
        float angle = emitted * 2 * PI / 10;
        particles[i] = {origin,
                        {std::cos(angle) * 58, std::sin(angle) * 58},
                        Config::ParticleSeconds,
                        parcel.pointsAwarded > 0 ? Ui::Cyan : Ui::destinationColor(Destination::Red)};
        ++emitted;
    }
}
void Game::syncParcels() {
    const HashTable& registry = simulation.dispatch.registry;
    for (int i = 0; i < registry.getCapacity(); ++i) {
        const Bucket& bucket = registry.getBucket(i);
        if (bucket.state != BucketState::Occupied) {
            continue;
        }
        const Parcel& parcel = bucket.parcel;
        ParcelMotion& m = motion[parcel.id];
        if (parcel.status == ParcelStatus::OnConveyor && m.status != ParcelStatus::OnConveyor) {
            for (int lane = 0; lane < simulation.conveyor.layout.inputCount; lane++) {
                if (parcel.cell == simulation.conveyor.layout.inputs[lane]) {
                    lastEntered[lane] = parcel.id;
                }
            }
        }
        int target = parcel.status == ParcelStatus::OnConveyor ? parcel.cell : parcel.eventCell;
        if (target >= 0 && target != m.targetCell) {
            Vector2 position = Renderer::center(target);
            if (m.targetCell < 0) {
                m.position = position;
            }
            m.from = m.position;
            m.target = position;
            m.targetCell = target;
            m.elapsed = 0;
            m.duration = Config::TickSeconds * simulation.conveyor.layout.tuning.moveTicks *
                         Config::ParcelTravelFraction;
        }
        if (parcel.status != m.status && terminal(parcel.status)) {
            // Observe every terminal transition, even several events in one tick.
            m.outcomePending = true;
            if (parcel.status == ParcelStatus::Expired) {
                m.target = m.from = m.position;
                m.elapsed = m.duration = 0;
            }
        }
        m.status = parcel.status;
    }
    int multiplier = simulation.dispatch.multiplier();
    if (multiplier > comboShown) {
        comboTimer = Config::PopupSeconds;
    } else if (multiplier < comboShown) {
        comboTimer = 0;
    }
    comboShown = multiplier;
}
void Game::effects(float dt) {
    bool running = screen == Screen::Playing && !paused && !demos.mode;
    if (!running && !(screen == Screen::Results && !demos.mode)) {
        return;
    }
    if (running) {
        beltClock += dt;
        titleTimer -= dt;
        if (titleTimer < 0) {
            titleTimer = 0;
        }
        const LevelDefinition& layout = simulation.conveyor.layout;
        for (int i = 0; i < Config::Cells; ++i) {
            const Cell& cell = layout.cells[i];
            if (cell.brakeUntil > simulation.tick) {
                continue;
            }
            for (int branch = 0; branch < 2; ++branch) {
                int target = cell.next[branch];
                if (target >= 0 && layout.cells[target].brakeUntil <= simulation.tick) {
                    beltOffset[i][branch] = std::fmod(
                        beltOffset[i][branch] + dt * Config::BeltPixelsPerSecond, (float)(Config::Tile));
                }
            }
        }
    }
    comboTimer -= dt;
    if (comboTimer < 0) {
        comboTimer = 0;
    }
    for (int i = 1; i <= Config::MaxParcels; ++i) {
        ParcelMotion& m = motion[i];
        if (m.outcomePending && m.elapsed + dt >= m.duration) {
            const Parcel* p = simulation.dispatch.registry.find(i);
            if (p) {
                deliveryEffect(*p);
            }
            m.outcomePending = false;
        }
        if (m.targetCell < 0) {
            continue;
        }
        m.elapsed += dt;
        float fraction = 1;
        if (m.duration > 0) {
            fraction = m.elapsed / m.duration;
        }
        if (fraction > 1) {
            fraction = 1;
        }
        m.position = {m.from.x + (m.target.x - m.from.x) * fraction,
                      m.from.y + (m.target.y - m.from.y) * fraction};
        if (m.status == ParcelStatus::OnConveyor) {
            m.opacity += dt * 8;
            if (m.opacity > 1) {
                m.opacity = 1;
            }
        } else if (fraction >= 1) {
            m.opacity -= dt / Config::ParcelFadeSeconds;
            if (m.opacity < 0) {
                m.opacity = 0;
            }
        }
    }
    for (int i = 0; i < Config::PopupCount; ++i) {
        if (popups[i].life <= 0) {
            continue;
        }
        popups[i].life -= dt;
        popups[i].position.y -= 25 * dt;
    }
    for (int i = 0; i < Config::ParticleCount; ++i) {
        Particle& p = particles[i];
        if (p.life <= 0) {
            continue;
        }
        p.life -= dt;
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.velocity.y += 65 * dt;
    }
}
bool Game::run() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(Config::WindowWidth, Config::WindowHeight, "Parcel Panic: Conveyor Mayhem");
    if (!IsWindowReady()) {
        return false;
    }
    Ui::loadFonts();
    SetExitKey(KEY_NULL);
    SetTargetFPS(Config::RenderFPS);
    while (!WindowShouldClose() && !quitting) {
        float dt = GetFrameTime(); // Seconds since the last frame.
        if (dt > Config::MaxFrameDelta) {
            dt = Config::MaxFrameDelta;
        }
        clock += dt;
        input();
        turnJunctions(dt);
        float remaining = dt;
        // Advance animation up to each tick boundary before applying the next move.
        // This avoids a position jump when a frame crosses one or more ticks.
        while (remaining > 0 && screen == Screen::Playing && !paused && !demos.mode) {
            float slice = Config::TickSeconds - accumulator;
            if (slice > remaining) {
                slice = remaining;
            }
            effects(slice);
            accumulator += slice;
            remaining -= slice;
            if (accumulator >= Config::TickSeconds) {
                accumulator = 0;
                simulation.step();
                syncParcels();
                if (simulation.outcome != Outcome::Playing) {
                    screen = Screen::Results;
                }
            }
        }
        if (remaining > 0) {
            effects(remaining);
        }
        inspectParcel(dt);
        BeginDrawing();
        Renderer::draw(*this);
        EndDrawing();
    }
    Ui::unloadFonts();
    CloseWindow();
    return true;
}
