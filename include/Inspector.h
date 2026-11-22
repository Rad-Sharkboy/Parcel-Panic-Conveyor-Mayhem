#ifndef INSPECTOR_H
#define INSPECTOR_H

class Game;

// View state only. Simulation owns every parcel and every displayed structure.
// Tabs: 0 = CircularQueue, 1 = MinHeap, 2 = HashTable, 3 = LinkedStack.
class Inspector {
public:
    bool open = false;
    int tab = 0, page = 0, selectedId = 0, searchId = 0, junction = 0;

    // Handle keyboard/mouse navigation, live hash lookup, and paused single-tick stepping.
    void input(Game& game);
    // Render the tabbed live data-structure inspector overlay and recent operation log.
    void draw(const Game& game) const;
};

#endif