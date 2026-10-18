#ifndef SIMULATION_H
#define SIMULATION_H // Prevent this header being included twice in the same source file.
#include "ConveyorSystem.h"
enum class Outcome { Playing, Won, Lost };
class Simulation {
    int nextSpawnTick = 0;

public:
    ConveyorSystem conveyor;
    DispatchSystem dispatch;
    int level = 0, tick = 0, generated = 0, jam = 0;
    Outcome outcome = Outcome::Playing;
    void load(int index);
    void next(int delta = 1);
    void step();
    bool rush() const;
    bool rushWarning() const;
    // Deterministic parcel creation through the real input queues; false when buffer is full.
    bool spawn();
};

#endif
