#ifndef INSPECTOR_H
#define INSPECTOR_H
class Game;

// View state only. Simulation owns every parcel and every displayed structure.
class Inspector {
public:
    bool open = false;
    int tab = 0, page = 0, selectedId = 0, searchId = 0, junction = 0;
    void input(Game& game);
    void draw(const Game& game) const;
};
#endif
