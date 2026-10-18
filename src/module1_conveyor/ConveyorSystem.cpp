#include "ConveyorSystem.h"
void ConveyorSystem::reset(int level) {
    layout = makeLevel(level);
    for (int i = 0; i < Config::MaxInputs; ++i) {
        inputs[i].clear();
    }
    history.clear();
    blocked = brakeReady = 0;
}
bool ConveyorSystem::switchJunction(int cellIndex, bool planning) {
    if (cellIndex < 0 || cellIndex >= Config::Cells || layout.cells[cellIndex].kind != CellKind::Junction) {
        return false;
    }
    Cell& cell = layout.cells[cellIndex];
    if (planning) {
        history.push({cellIndex, cell.choice});
    }
    cell.choice = 1 - cell.choice;
    return true;
}
bool ConveyorSystem::undo() {
    if (history.isEmpty()) {
        return false;
    }
    SwitchCommand command = history.pop();
    layout.cells[command.cell].choice = command.oldChoice;
    return true;
}
bool ConveyorSystem::brake(int cell, int tick) {
    if (tick < brakeReady || cell < 0 || cell >= Config::Cells ||
        layout.cells[cell].kind == CellKind::Empty || layout.cells[cell].kind == CellKind::Dock) {
        return false;
    }
    layout.cells[cell].brakeUntil = tick + Config::BrakeTicks;
    brakeReady = tick + Config::BrakeCooldown;
    return true;
}
void ConveyorSystem::feed(HashTable& registry, int tick) {
    for (int i = 0; i < layout.inputCount; ++i) {
        Cell& cell = layout.cells[layout.inputs[i]];
        int id = 0;
        while (!inputs[i].isEmpty()) {
            id = inputs[i].peek();
            Parcel* parcel = registry.find(id);
            if (parcel && parcel->status == ParcelStatus::Queued) {
                break;
            }
            inputs[i].dequeue();
        }
        if (cell.parcel != 0 || cell.brakeUntil > tick || inputs[i].isEmpty()) {
            continue;
        }
        id = inputs[i].dequeue();
        Parcel* parcel = registry.find(id);
        if (!parcel) {
            continue;
        }
        cell.parcel = id;
        parcel->status = ParcelStatus::OnConveyor;
        parcel->cell = parcel->previousCell = layout.inputs[i];
        parcel->movedTick = tick;
    }
}
void ConveyorSystem::move(HashTable& registry, DispatchSystem& dispatch, int tick) {
    int occupancy[Config::Cells]{};
    bool claimed[Config::Cells]{};
    blocked = 0;
    for (int i = 0; i < Config::Cells; ++i) {
        Cell& cell = layout.cells[i];
        const Parcel* parcel = registry.find(cell.parcel);
        if (cell.parcel && (!parcel || parcel->status != ParcelStatus::OnConveyor)) {
            cell.parcel = 0;
        }
        occupancy[i] = cell.parcel;
    }
    // Rotating scan gives each merge branch a deterministic chance to enter.
    for (int offset = 0; offset < Config::Cells; ++offset) {
        int i = (offset + tick) % Config::Cells;
        if (!occupancy[i]) {
            continue;
        }
        Cell& cell = layout.cells[i];
        Parcel* parcel = registry.find(occupancy[i]);
        if (cell.brakeUntil > tick) {
            continue;
        }
        if (cell.kind == CellKind::Dock) {
            cell.parcel = 0;
            dispatch.arrive(parcel->id, cell.dock, tick);
            continue;
        }
        int target = cell.next[cell.kind == CellKind::Junction ? cell.choice : 0];
        if (target < 0 || target >= Config::Cells || layout.cells[target].kind == CellKind::Empty) {
            ++blocked;
            continue;
        }
        if (occupancy[target] || claimed[target] || layout.cells[target].brakeUntil > tick) {
            ++blocked;
            continue;
        }
        claimed[target] = true;
        cell.parcel = 0;
        Cell& nextCell = layout.cells[target];
        nextCell.parcel = parcel->id;
        parcel->previousCell = i;
        parcel->cell = target;
        parcel->movedTick = tick;
        if (nextCell.kind == CellKind::Hazard && parcel->kind == ParcelKind::Fragile) {
            nextCell.parcel = 0;
            dispatch.fail(parcel->id, ParcelStatus::Broken, tick);
        }
    }
}
