#include "Simulation.h"

void Simulation::load(int index) {
    // Double modulo wraps negative indices cleanly (e.g., Shift+F4 from level 0).
    level = (index % Config::Levels + Config::Levels) % Config::Levels;
    conveyor.reset(level);
    dispatch.reset();
    tick = 0;
    generated = 0;
    jam = 0;
    nextSpawnTick = 0;
    outcome = Outcome::Playing;
}

void Simulation::next(int delta) {
    load(level + delta);
}

bool Simulation::rush() const {
    int phase = tick % Config::RushPeriod;
    return conveyor.layout.tuning.rush && phase >= Config::RushPeriod - Config::RushDuration;
}

bool Simulation::rushWarning() const {
    int phase = tick % Config::RushPeriod;
    int warningStart = Config::RushPeriod - Config::RushDuration - Config::RushWarning;
    return conveyor.layout.tuning.rush && phase >= warningStart && !rush();
}

bool Simulation::spawn() {
    const Config::LevelTuning& tuning = conveyor.layout.tuning;
    if (generated >= tuning.total || generated >= Config::MaxParcels ||
        conveyor.layout.inputCount <= 0) {
        return false;
    }
    int inputLane = generated % conveyor.layout.inputCount;
    if (conveyor.inputs[inputLane].isFull()) {
        return false;
    }

    Parcel parcel;
    parcel.id = generated + 1;
    parcel.destination = (Destination)(generated % tuning.colors);
    if (tuning.specials) {
        if ((level != 2 || generated >= Config::TeachDamagedAfter) &&
            generated % Config::DamagedPeriod == Config::DamagedOffset) {
            parcel.kind = ParcelKind::Damaged;
            parcel.destination = Destination::Recycle;
        } else if ((level != 2 || generated >= Config::TeachFragileAfter) &&
                   generated % Config::FragilePeriod == Config::FragileOffset) {
            parcel.kind = ParcelKind::Fragile;
        } else if ((level != 2 || generated >= Config::TeachExpressAfter) &&
                   generated % Config::ExpressPeriod == Config::ExpressOffset) {
            parcel.kind = ParcelKind::Express;
        }
    }

    parcel.deadline = (parcel.kind == ParcelKind::Express)
                          ? (tick + Config::ExpressDeadlineTicks)
                          : (tick + Config::DeadlineTicks);
    parcel.status = ParcelStatus::Queued;

    // Register parcel in Module 2 HashTable and log rehash events if capacity expands.
    int previousCapacity = dispatch.registry.getCapacity();
    dispatch.registry.insert(parcel);
    if (dispatch.registry.getCapacity() != previousCapacity) {
        dispatch.activity.add(Operation::Rehash, dispatch.registry.getCapacity(), previousCapacity);
    }
    dispatch.activity.add(Operation::Register, parcel.id);

    // Enqueue parcel ID into the assigned Module 1 CircularQueue input lane.
    conveyor.inputs[inputLane].enqueue(parcel.id);
    conveyor.activity.add(Operation::Enqueue, parcel.id, inputLane);
    generated++;
    return true;
}

void Simulation::step() {
    if (outcome != Outcome::Playing) {
        return;
    }
    tick++;

    // Undo stack only applies to paused junction edits; clear it once simulation advances.
    if (!conveyor.history.isEmpty()) {
        conveyor.activity.add(Operation::ClearUndo, conveyor.history.size());
        conveyor.history.clear();
    }

    const Config::LevelTuning& tuning = conveyor.layout.tuning;
    if (tick >= nextSpawnTick && generated < tuning.total) {
        if (spawn()) {
            int spawnInterval = tuning.spawnTicks;
            if (rush()) {
                spawnInterval = spawnInterval / Config::RushSpawnDivisor;
                if (spawnInterval < Config::RushMinSpawnTicks) {
                    spawnInterval = Config::RushMinSpawnTicks;
                }
            }
            nextSpawnTick = tick + spawnInterval;
        }
    }

    // Advance Module 2 deadline checks and priority MinHeap dispatch.
    dispatch.update(tick, tuning.loaderTicks);

    // Advance Module 1 conveyor cells and recalculate congestion/jam meter.
    if (tuning.moveTicks > 0 && tick % tuning.moveTicks == 0) {
        conveyor.move(dispatch.registry, dispatch, tick);
        conveyor.feed(dispatch.registry, tick);

        int queuedWaiting = 0;
        for (int i = 0; i < conveyor.layout.inputCount; i++) {
            queuedWaiting += conveyor.inputs[i].size();
        }
        int congestionScore = conveyor.blocked;
        if (queuedWaiting >= Config::InputCapacity) {
            congestionScore += 2;
        }
        if (congestionScore >= Config::JamBlockedThreshold) {
            jam += Config::JamRise;
        } else {
            jam -= Config::JamRecovery;
        }
        if (jam < 0) {
            jam = 0;
        }
        if (jam > Config::JamLimit) {
            jam = Config::JamLimit;
        }
    }

    // Evaluate terminal win/loss conditions.
    if (jam >= Config::JamLimit || tick >= Config::MaxLevelTicks) {
        outcome = Outcome::Lost;
    } else if (generated == tuning.total && dispatch.delivered + dispatch.failed == generated) {
        if (dispatch.delivered >= tuning.quota && dispatch.score >= tuning.target) {
            outcome = Outcome::Won;
        } else {
            outcome = Outcome::Lost;
        }
    }
}