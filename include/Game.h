#ifndef GAME_H
#define GAME_H // Prevent this header being included twice in the same source file.
#include "Simulation.h"
#include "Demos.h"
#include "raylib.h"
enum class Screen { Menu, Select, Help, Playing, Results };
struct Particle { Vector2 position{}, velocity{}; float life = 0; Color color{}; };
struct ParcelMotion {
    Vector2 from{}, position{}, target{};
    float elapsed = 0, duration = 0, opacity = 0;
    int targetCell = -1;
    ParcelStatus status = ParcelStatus::Created;
    bool outcomePending = false;
};
struct ScorePopup { Vector2 position{}; float life = 0; int points = 0; };
class Game {
    bool quitting_ = false;
    void input();
    void effects(float dt);
    void syncParcels();
    void deliveryEffect(const Parcel& parcel);
    void turnJunctions(float dt);
public:
    Simulation simulation;
    Demos demos;
    Screen screen = Screen::Menu;
    bool paused = false, hud = true;
    float accumulator = 0, clock = 0, beltClock = 0, titleTimer = 0, comboTimer = 0;
    int comboShown = 1;
    int selectedLevel = 0;
    int hoverCell = -1;
    float junctionPulse[Config::Cells]{};
    float junctionAngle[Config::Cells]{};
    int junctionChoice[Config::Cells]{};
    float beltOffset[Config::Cells][2]{};
    ParcelMotion motion[Config::MaxParcels + 1]{};
    ScorePopup popups[Config::PopupCount]{};
    Particle particles[Config::ParticleCount]{};
    Game();
    void start(int level);
    bool run(int smokeFrames = 0, const char* screenshot = nullptr, const char* stateFile = nullptr);
};

#endif
