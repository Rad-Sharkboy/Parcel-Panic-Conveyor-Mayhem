#include "Level.h"
namespace {
void path(LevelDefinition& layout, int x1, int y1, int x2, int y2) {
    int x = x1, y = y1;
    while (x != x2 || y != y2) {
        int nextX = x + (x < x2 ? 1 : x > x2 ? -1 : 0);
        int nextY = y + (y < y2 ? 1 : y > y2 ? -1 : 0);
        Cell& cell = layout.cells[cellAt(x, y)];
        cell.kind = CellKind::Belt;
        cell.next[0] = cellAt(nextX, nextY);
        x = nextX;
        y = nextY;
    }
}
void dock(LevelDefinition& layout, int x, int y, Destination destination) {
    Cell& cell = layout.cells[cellAt(x, y)];
    cell.kind = CellKind::Dock;
    cell.dock = destination;
}
void junction(LevelDefinition& layout, int x, int y, int ax, int ay, int bx, int by) {
    int i = cellAt(x, y);
    Cell& cell = layout.cells[i];
    cell.kind = CellKind::Junction;
    cell.next[0] = cellAt(ax, ay);
    cell.next[1] = cellAt(bx, by);
    layout.junctions[layout.junctionCount++] = i;
}
void input(LevelDefinition& layout, int x, int y) {
    layout.inputs[layout.inputCount++] = cellAt(x, y);
}
void mergeInputs(LevelDefinition& layout, int top, int bottom, int mergeX) {
    path(layout, 0, top, mergeX, top);
    path(layout, mergeX, top, mergeX, 5);
    path(layout, 0, bottom, mergeX, bottom);
    path(layout, mergeX, bottom, mergeX, 5);
    input(layout, 0, top);
    input(layout, 0, bottom);
}
} // namespace
LevelDefinition makeLevel(int index) {
    LevelDefinition layout;
    index = (index % Config::Levels + Config::Levels) % Config::Levels;
    layout.tuning = Config::LevelsTuning[index];
    if (index == 0) {
        layout.name = "Sort School";
        layout.lesson = "Two colors. Click cyan junctions; follow the matching letter docks.";
        mergeInputs(layout, 3, 7, 3);
        path(layout, 3, 5, 15, 5);
        path(layout, 8, 4, 8, 2);
        path(layout, 8, 2, 15, 2);
        dock(layout, 15, 2, Destination::Red);
        path(layout, 12, 6, 12, 8);
        path(layout, 12, 8, 15, 8);
        dock(layout, 15, 8, Destination::Blue);
        dock(layout, 15, 5, Destination::Blue);
        junction(layout, 8, 5, 9, 5, 8, 4);
        junction(layout, 12, 5, 13, 5, 12, 6);
    } else if (index == 1) {
        layout.name = "Rush Junction";
        layout.lesson = "Three merging inputs. Watch the rush warning and stagger streams with brakes.";
        mergeInputs(layout, 2, 8, 3);
        path(layout, 0, 5, 15, 5);
        input(layout, 0, 5);
        path(layout, 6, 4, 6, 1);
        path(layout, 6, 1, 15, 1);
        dock(layout, 15, 1, Destination::Red);
        path(layout, 10, 6, 10, 9);
        path(layout, 10, 9, 15, 9);
        dock(layout, 15, 9, Destination::Blue);
        dock(layout, 15, 5, Destination::Yellow);
        junction(layout, 6, 5, 7, 5, 6, 4);
        junction(layout, 10, 5, 11, 5, 10, 6);
    } else if (index == 2) {
        layout.name = "Fragile Freight";
        layout.lesson = "F parcels need the upper bypass. X parcels go to green recycling; ! is express.";
        mergeInputs(layout, 2, 8, 3);
        path(layout, 3, 5, 15, 5);
        path(layout, 6, 4, 6, 3);
        path(layout, 6, 3, 9, 3);
        path(layout, 9, 3, 9, 5);
        layout.cells[cellAt(7, 5)].kind = CellKind::Hazard;
        junction(layout, 6, 5, 7, 5, 6, 4);
        path(layout, 10, 4, 10, 1);
        path(layout, 10, 1, 15, 1);
        dock(layout, 15, 1, Destination::Red);
        path(layout, 12, 6, 12, 9);
        path(layout, 12, 9, 15, 9);
        dock(layout, 15, 9, Destination::Blue);
        path(layout, 14, 4, 14, 3);
        path(layout, 14, 3, 15, 3);
        dock(layout, 15, 3, Destination::Yellow);
        dock(layout, 15, 5, Destination::Recycle);
        junction(layout, 10, 5, 11, 5, 10, 4);
        junction(layout, 12, 5, 13, 5, 12, 6);
        junction(layout, 14, 5, 15, 5, 14, 4);
    } else {
        layout.name = "Mayhem Mode";
        layout.lesson = "Vertical sorting spine, three inputs, all parcel types and a slower shared loader.";
        path(layout, 0, 1, 8, 1);
        path(layout, 8, 1, 8, 9);
        path(layout, 8, 9, 15, 9);
        path(layout, 0, 3, 8, 3);
        path(layout, 0, 5, 8, 5);
        input(layout, 0, 1);
        input(layout, 0, 3);
        input(layout, 0, 5);
        path(layout, 4, 2, 4, 3);
        layout.cells[cellAt(6, 1)].kind = CellKind::Hazard;
        junction(layout, 4, 1, 5, 1, 4, 2);
        path(layout, 9, 6, 15, 6);
        dock(layout, 15, 6, Destination::Red);
        path(layout, 7, 7, 4, 7);
        path(layout, 4, 7, 4, 10);
        path(layout, 4, 10, 6, 10);
        dock(layout, 6, 10, Destination::Blue);
        path(layout, 9, 8, 15, 8);
        dock(layout, 15, 8, Destination::Yellow);
        dock(layout, 15, 9, Destination::Recycle);
        junction(layout, 8, 6, 8, 7, 9, 6);
        junction(layout, 8, 7, 8, 8, 7, 7);
        junction(layout, 8, 8, 8, 9, 9, 8);
    }
    return layout;
}
