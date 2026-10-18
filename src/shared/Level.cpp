#include "Level.h"
namespace {
void path(LevelDefinition& l, int x1, int y1, int x2, int y2) {
    int x = x1, y = y1;
    while (x != x2 || y != y2) {
        int nx = x + (x < x2 ? 1 : x > x2 ? -1 : 0);
        int ny = y + (y < y2 ? 1 : y > y2 ? -1 : 0);
        Cell& c = l.cells[cellAt(x, y)]; c.kind = CellKind::Belt; c.next[0] = cellAt(nx, ny);
        x = nx; y = ny;
    }
}
void dock(LevelDefinition& l, int x, int y, Destination destination) {
    Cell& c = l.cells[cellAt(x, y)]; c.kind = CellKind::Dock; c.dock = destination;
}
void junction(LevelDefinition& l, int x, int y, int ax, int ay, int bx, int by) {
    int i = cellAt(x, y); Cell& c = l.cells[i]; c.kind = CellKind::Junction;
    c.next[0] = cellAt(ax, ay); c.next[1] = cellAt(bx, by); l.junctions[l.junctionCount++] = i;
}
void input(LevelDefinition& l, int x, int y) { l.inputs[l.inputCount++] = cellAt(x, y); }
void mergeInputs(LevelDefinition& l, int top, int bottom, int mergeX) {
    path(l, 0, top, mergeX, top); path(l, mergeX, top, mergeX, 5);
    path(l, 0, bottom, mergeX, bottom); path(l, mergeX, bottom, mergeX, 5);
    input(l, 0, top); input(l, 0, bottom);
}
}
LevelDefinition makeLevel(int index) {
    LevelDefinition l; index = (index % Config::Levels + Config::Levels) % Config::Levels;
    l.tuning = Config::LevelsTuning[index];
    if (index == 0) {
        l.name = "Sort School"; l.lesson = "Two colors. Click cyan junctions; follow the matching letter docks.";
        mergeInputs(l, 3, 7, 3); path(l, 3, 5, 15, 5);
        path(l, 8, 4, 8, 2); path(l, 8, 2, 15, 2); dock(l, 15, 2, Destination::Red);
        path(l, 12, 6, 12, 8); path(l, 12, 8, 15, 8); dock(l, 15, 8, Destination::Blue);
        dock(l, 15, 5, Destination::Blue);
        junction(l, 8, 5, 9, 5, 8, 4); junction(l, 12, 5, 13, 5, 12, 6);
    } else if (index == 1) {
        l.name = "Rush Junction"; l.lesson = "Three merging inputs. Watch the rush warning and stagger streams with brakes.";
        mergeInputs(l, 2, 8, 3); path(l, 0, 5, 15, 5); input(l, 0, 5);
        path(l, 6, 4, 6, 1); path(l, 6, 1, 15, 1); dock(l, 15, 1, Destination::Red);
        path(l, 10, 6, 10, 9); path(l, 10, 9, 15, 9); dock(l, 15, 9, Destination::Blue);
        dock(l, 15, 5, Destination::Yellow);
        junction(l, 6, 5, 7, 5, 6, 4); junction(l, 10, 5, 11, 5, 10, 6);
    } else if (index == 2) {
        l.name = "Fragile Freight"; l.lesson = "F parcels need the upper bypass. X parcels go to green recycling; ! is express.";
        mergeInputs(l, 2, 8, 3); path(l, 3, 5, 15, 5);
        path(l, 6, 4, 6, 3); path(l, 6, 3, 9, 3); path(l, 9, 3, 9, 5);
        l.cells[cellAt(7, 5)].kind = CellKind::Hazard;
        junction(l, 6, 5, 7, 5, 6, 4);
        path(l, 10, 4, 10, 1); path(l, 10, 1, 15, 1); dock(l, 15, 1, Destination::Red);
        path(l, 12, 6, 12, 9); path(l, 12, 9, 15, 9); dock(l, 15, 9, Destination::Blue);
        path(l, 14, 4, 14, 3); path(l, 14, 3, 15, 3); dock(l, 15, 3, Destination::Yellow);
        dock(l, 15, 5, Destination::Recycle);
        junction(l, 10, 5, 11, 5, 10, 4); junction(l, 12, 5, 13, 5, 12, 6); junction(l, 14, 5, 15, 5, 14, 4);
    } else {
        l.name = "Mayhem Mode"; l.lesson = "Vertical sorting spine, three inputs, all parcel types and a slower shared loader.";
        path(l, 0, 1, 8, 1); path(l, 8, 1, 8, 9); path(l, 8, 9, 15, 9);
        path(l, 0, 3, 8, 3); path(l, 0, 5, 8, 5);
        input(l, 0, 1); input(l, 0, 3); input(l, 0, 5);
        path(l, 4, 2, 4, 3); l.cells[cellAt(6, 1)].kind = CellKind::Hazard;
        junction(l, 4, 1, 5, 1, 4, 2);
        path(l, 9, 6, 15, 6); dock(l, 15, 6, Destination::Red);
        path(l, 7, 7, 4, 7); path(l, 4, 7, 4, 10); path(l, 4, 10, 6, 10); dock(l, 6, 10, Destination::Blue);
        path(l, 9, 8, 15, 8); dock(l, 15, 8, Destination::Yellow); dock(l, 15, 9, Destination::Recycle);
        junction(l, 8, 6, 8, 7, 9, 6); junction(l, 8, 7, 8, 8, 7, 7); junction(l, 8, 8, 8, 9, 9, 8);
    }
    return l;
}
