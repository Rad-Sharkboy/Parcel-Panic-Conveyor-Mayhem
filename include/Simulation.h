#ifndef SIMULATION_H
#define SIMULATION_H // Prevent this header being included twice in the same source file.

#include "ConveyorSystem.h"

enum class Outcome { Playing, Won, Lost };

// Coordinates Module 1 (ConveyorSystem) and Module 2 (DispatchSystem) on a deterministic tick loop.
class Simulation {
private:
    int nextSpawnTick = 0; // Next simulation tick scheduled to spawn an incoming parcel.

public:
    ConveyorSystem conveyor;
    DispatchSystem dispatch;
    int level = 0, tick = 0, generated = 0, jam = 0;
    Outcome outcome = Outcome::Playing;

    // Initialize level configuration and reset conveyor/dispatch state.
    void load(int index);
    // Advance the simulation by 'delta' discrete ticks while Outcome is Playing.
    void next(int delta = 1);
    // Execute one full tick: spawn, conveyor movement, dock arrivals, and heap dispatch.
    void step();
    // True when current tick falls inside the level's high-traffic rush window.
    bool rush() const;
    // True shortly before rush hour begins to alert the player/HUD.
    bool rushWarning() const;
    // Deterministic parcel creation through the real input queues; false when buffer is full.
    bool spawn();
};

#endif