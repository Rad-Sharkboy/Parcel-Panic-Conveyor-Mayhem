#pragma once
#include "Simulation.h"
#include "Demos.h"
#include "raylib.h"
enum class Screen { Menu, Select, Help, Playing, Results };
struct Particle { Vector2 position{}, velocity{}; float life = 0; Color color{}; };
class Game {
    bool quitting_ = false;
    void input();
    void effects(float dt);
    void deliveryEffect();
    int lastEffectTick_ = -100;
public:
    Simulation simulation;
    Demos demos;
    Screen screen = Screen::Menu;
    bool paused = false, hud = true;
    float accumulator = 0, clock = 0, titleTimer = 0;
    int selectedLevel = 0;
    int hoverCell = -1;
    float junctionPulse[Config::Cells]{};
    Particle particles[Config::ParticleCount]{};
    Game();
    void start(int level);
    bool run(int smokeFrames = 0, const char* screenshot = nullptr, const char* stateFile = nullptr);
};
