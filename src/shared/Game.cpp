#include "Game.h"
#include "Renderer.h"
#include <algorithm>
#include <cmath>
#include <fstream>
Game::Game() { simulation.load(0); }
void Game::start(int level) {
    simulation.load(level); selectedLevel = simulation.level; screen = Screen::Playing;
    paused = false; demos.mode = 0; accumulator = 0; titleTimer = 2.5f; lastEffectTick_ = -100;
    for (int i = 0; i < Config::Cells; ++i) junctionPulse[i] = 0;
    for (int i = 0; i < Config::ParticleCount; ++i) particles[i].life = 0;
}
void Game::input() {
    bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    if (IsKeyPressed(KEY_F4)) { start(simulation.level + (shift ? -1 : 1)); return; }
    if (IsKeyPressed(KEY_F5) || (IsKeyPressed(KEY_R) && (screen == Screen::Playing || screen == Screen::Results))) { start(simulation.level); return; }
    for (int i = 1; i <= 3; ++i) {
        if (IsKeyPressed(KEY_F1 + i - 1)) { if (demos.mode == i) demos.mode = 0; else demos.open(i); return; }
    }
    if (demos.mode) {
        if (IsKeyPressed(KEY_ESCAPE)) demos.mode = 0;
        else if (IsKeyPressed(KEY_BACKSPACE)) demos.open(demos.mode);
        else if (IsKeyPressed(KEY_ENTER)) demos.advance();
        return;
    }
    if (IsKeyPressed(KEY_H)) hud = !hud;
    if (screen == Screen::Menu) {
        if (IsKeyPressed(KEY_ENTER) || Renderer::clicked(82, 340, 320, 56)) start(selectedLevel);
        else if (Renderer::clicked(82, 410, 320, 50)) screen = Screen::Select;
        else if (Renderer::clicked(82, 474, 320, 50)) screen = Screen::Help;
        else if (Renderer::clicked(82, 538, 320, 50) || IsKeyPressed(KEY_ESCAPE)) quitting_ = true;
        return;
    }
    if (screen == Screen::Select) {
        for (int i = 0; i < Config::Levels; ++i) if (Renderer::clicked(70.0f + i * 295, 260, 275, 270) || IsKeyPressed(KEY_ONE + i)) { start(i); return; }
        if (IsKeyPressed(KEY_ESCAPE) || Renderer::clicked(70, 580, 220, 48)) screen = Screen::Menu;
        return;
    }
    if (screen == Screen::Help) {
        if (IsKeyPressed(KEY_ESCAPE) || Renderer::clicked(70, 600, 220, 48)) screen = Screen::Menu;
        return;
    }
    if (screen == Screen::Results) {
        if (Renderer::clicked(390, 440, 230, 52)) start(simulation.level);
        else if (Renderer::clicked(650, 440, 230, 52) || IsKeyPressed(KEY_ENTER)) start(simulation.level + 1);
        else if (Renderer::clicked(520, 510, 230, 42) || IsKeyPressed(KEY_ESCAPE)) screen = Screen::Menu;
        return;
    }
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) { paused = !paused; }
    if (paused && IsKeyPressed(KEY_Z)) simulation.conveyor.undo();
    if (paused) {
        if (Renderer::clicked(890, 440, 160, 44)) paused = false;
        else if (Renderer::clicked(1060, 440, 160, 44)) { start(simulation.level); return; }
        else if (Renderer::clicked(890, 495, 330, 38)) { screen = Screen::Menu; return; }
    }
    Vector2 mouse = GetMousePosition();
    int x = static_cast<int>((mouse.x - Config::BoardX) / Config::Tile);
    int y = static_cast<int>((mouse.y - Config::BoardY) / Config::Tile);
    hoverCell = mouse.x >= Config::BoardX && mouse.y >= Config::BoardY && x >= 0 && x < Config::Columns && y >= 0 && y < Config::Rows ? cellAt(x, y) : -1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && simulation.conveyor.switchJunction(hoverCell, paused)) junctionPulse[hoverCell] = 1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) simulation.conveyor.brake(hoverCell, simulation.tick);
}
void Game::deliveryEffect() {
    const DispatchSystem& d = simulation.dispatch;
    if (d.lastEventTick == lastEffectTick_ || d.lastEventTick < 0) return;
    lastEffectTick_ = d.lastEventTick;
    const Parcel* p = d.registry.find(d.lastEventId);
    Vector2 origin = Renderer::center(p && p->previousCell >= 0 ? p->previousCell : simulation.conveyor.layout.inputs[0]);
    int emitted = 0;
    for (int i = 0; i < Config::ParticleCount && emitted < 14; ++i) {
        if (particles[i].life > 0) continue;
        float angle = static_cast<float>(emitted) * 6.283185f / 14;
        particles[i] = {origin, {std::cos(angle) * 80, std::sin(angle) * 80}, 0.65f, d.lastPoints > 0 ? Color{85, 245, 183, 255} : Color{255, 92, 95, 255}}; ++emitted;
    }
}
void Game::effects(float dt) {
    clock += dt; titleTimer = std::max(0.0f, titleTimer - dt);
    for (int i = 0; i < Config::Cells; ++i) junctionPulse[i] = std::max(0.0f, junctionPulse[i] - dt * 3);
    for (int i = 0; i < Config::ParticleCount; ++i) {
        Particle& p = particles[i]; if (p.life <= 0) continue;
        p.life -= dt; p.position.x += p.velocity.x * dt; p.position.y += p.velocity.y * dt; p.velocity.y += 70 * dt;
    }
}
bool Game::run(int smokeFrames, const char* screenshot, const char* stateFile) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(Config::WindowWidth, Config::WindowHeight, "Parcel Panic: Conveyor Mayhem");
    if (!IsWindowReady()) return false;
    SetExitKey(KEY_NULL); SetTargetFPS(Config::RenderFPS);
    if (smokeFrames > 0) start(selectedLevel);
    int frames = 0;
    bool success = true;
    while (!WindowShouldClose() && !quitting_) {
        float dt = std::min(GetFrameTime(), Config::MaxFrameDelta); input(); effects(dt);
        if (screen == Screen::Playing && !paused && !demos.mode) {
            accumulator += dt;
            while (accumulator >= Config::TickSeconds) {
                simulation.step(); accumulator -= Config::TickSeconds; deliveryEffect();
                if (simulation.outcome != Outcome::Playing) { screen = Screen::Results; break; }
            }
        } else if (screen != Screen::Playing) accumulator = 0;
        BeginDrawing(); Renderer::draw(*this); EndDrawing();
        // Optional integration-test observation; absent in normal play.
        if (stateFile) {
            std::ofstream state(stateFile);
            state << "{\"screen\":" << static_cast<int>(screen)
                  << ",\"level\":" << simulation.level + 1 << ",\"tick\":" << simulation.tick
                  << ",\"paused\":" << paused << ",\"demo\":" << demos.mode
                  << ",\"demoStep\":" << demos.step << ",\"undoDepth\":" << simulation.conveyor.history.size()
                  << ",\"brakeReady\":" << simulation.conveyor.brakeReady
                  << ",\"junctionChoice\":" << simulation.conveyor.layout.cells[simulation.conveyor.layout.junctions[0]].choice
                  << ",\"queueSize\":" << demos.queue.size() << ",\"heapSize\":" << demos.heap.size()
                  << ",\"hashCapacity\":" << demos.table.capacity() << "}\n";
        }
        ++frames;
        if (smokeFrames > 0 && frames >= smokeFrames) {
            if (screenshot) {
                Image capture = LoadImageFromScreen();
                success = ExportImage(capture, screenshot); UnloadImage(capture);
            }
            break;
        }
    }
    CloseWindow();
    return success;
}
