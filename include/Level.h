#pragma once
#include "Config.h"
#include "Parcel.h"
enum class CellKind { Empty, Belt, Junction, Dock, Hazard };
struct Cell {
    CellKind kind = CellKind::Empty;
    int next[2] = {-1, -1};
    int choice = 0;
    int parcel = 0;
    int brakeUntil = 0;
    Destination dock = Destination::Red;
};
struct LevelDefinition {
    const char* name = "";
    const char* lesson = "";
    Config::LevelTuning tuning{};
    Cell cells[Config::Cells]{};
    int inputs[Config::MaxInputs]{};
    int inputCount = 0;
    int junctions[Config::MaxJunctions]{};
    int junctionCount = 0;
};
inline int cellAt(int x, int y) { return y * Config::Columns + x; }
LevelDefinition makeLevel(int index);
