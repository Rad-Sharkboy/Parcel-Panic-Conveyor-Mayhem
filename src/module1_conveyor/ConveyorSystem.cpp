#include "ConveyorSystem.h"
void ConveyorSystem::reset(int level) {
    layout = makeLevel(level);
    for (int i = 0; i < Config::MaxInputs; ++i) {
        inputs[i].clear();
    }
    history.clear();
    blocked = brakeReady = 0;
}
bool ConveyorSystem::switchJunction(int cell, bool planning) {
    if (cell < 0 || cell >= Config::Cells || layout.cells[cell].kind != CellKind::Junction) {
        return false;
    }
    Cell& c = layout.cells[cell];
    if (planning) {
        history.push({cell, c.choice});
    }
    c.choice = 1 - c.choice;
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
            Parcel* p = registry.find(id);
            if (p && p->status == ParcelStatus::Queued) {
                break;
            }
            inputs[i].dequeue();
        }
        if (cell.parcel != 0 || cell.brakeUntil > tick || inputs[i].isEmpty()) {
            continue;
        }
        id = inputs[i].dequeue();
        Parcel* p = registry.find(id);
        if (!p) {
            continue;
        }
        cell.parcel = id;
        p->status = ParcelStatus::OnConveyor;
        p->cell = p->previousCell = layout.inputs[i];
        p->movedTick = tick;
    }
}
void ConveyorSystem::move(HashTable& registry, DispatchSystem& dispatch, int tick) {
    int occupancy[Config::Cells]{};
    bool claimed[Config::Cells]{};
    blocked = 0;
    for (int i = 0; i < Config::Cells; ++i) {
        Cell& c = layout.cells[i];
        const Parcel* p = registry.find(c.parcel);
        if (c.parcel && (!p || p->status != ParcelStatus::OnConveyor)) {
            c.parcel = 0;
        }
        occupancy[i] = c.parcel;
    }
    // Rotating scan gives each merge branch a deterministic chance to enter.
    for (int offset = 0; offset < Config::Cells; ++offset) {
        int i = (offset + tick) % Config::Cells;
        if (!occupancy[i]) {
            continue;
        }
        Cell& c = layout.cells[i];
        Parcel* p = registry.find(occupancy[i]);
        if (c.brakeUntil > tick) {
            continue;
        }
        if (c.kind == CellKind::Dock) {
            c.parcel = 0;
            dispatch.arrive(p->id, c.dock, tick);
            continue;
        }
        int target = c.next[c.kind == CellKind::Junction ? c.choice : 0];
        if (target < 0 || target >= Config::Cells || layout.cells[target].kind == CellKind::Empty) {
            ++blocked;
            continue;
        }
        if (occupancy[target] || claimed[target] || layout.cells[target].brakeUntil > tick) {
            ++blocked;
            continue;
        }
        claimed[target] = true;
        c.parcel = 0;
        Cell& next = layout.cells[target];
        next.parcel = p->id;
        p->previousCell = i;
        p->cell = target;
        p->movedTick = tick;
        if (next.kind == CellKind::Hazard && p->kind == ParcelKind::Fragile) {
            next.parcel = 0;
            dispatch.fail(p->id, ParcelStatus::Broken, tick);
        }
    }
}
