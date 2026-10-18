#ifndef CONVEYORSYSTEM_H
#define CONVEYORSYSTEM_H // Prevent this header being included twice in the same source file.
#include "Level.h"
#include "CircularQueue.h"
#include "CommandStack.h"
#include "DispatchSystem.h"
class ConveyorSystem {
public:
    LevelDefinition layout;
    CircularQueue inputs[Config::MaxInputs];
    CommandStack history;
    int blocked = 0;
    int brakeReady = 0;
    void reset(int level);
    bool switchJunction(int cell, bool planning);
    bool undo();
    bool brake(int cell, int tick);
    // Snapshot occupancy prevents two moves per tick and resolves merges by rotating priority.
    void move(HashTable& registry, DispatchSystem& dispatch, int tick);
    void feed(HashTable& registry, int tick);
};

#endif
