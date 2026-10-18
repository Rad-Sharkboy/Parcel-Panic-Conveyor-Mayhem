#include "Simulation.h"

void Simulation::load(int index) {
    // The second modulo also wraps negative indices (Shift+F4 from the first level).
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
    if (generated >= tuning.total || generated >= Config::MaxParcels) {
        return false;
    }
    int lane = generated % conveyor.layout.inputCount;
    if (conveyor.inputs[lane].isFull()) {
        return false;
    }

    Parcel parcel;
    parcel.id = generated + 1;
    parcel.destination = (Destination)(generated % tuning.colors);
    if (tuning.specials) {
        if (generated % Config::DamagedPeriod == Config::DamagedOffset) {
            parcel.kind = ParcelKind::Damaged;
            parcel.destination = Destination::Recycle;
        } else if (generated % Config::FragilePeriod == Config::FragileOffset) {
            parcel.kind = ParcelKind::Fragile;
        } else if (generated % Config::ExpressPeriod == Config::ExpressOffset) {
            parcel.kind = ParcelKind::Express;
        }
    }
    parcel.deadline = tick + Config::DeadlineTicks;
    if (parcel.kind == ParcelKind::Express) {
        parcel.deadline = tick + Config::ExpressDeadlineTicks;
    }
    parcel.status = ParcelStatus::Queued;
    dispatch.registry.insert(parcel);
    conveyor.inputs[lane].enqueue(parcel.id);
    generated++;
    return true;
}

void Simulation::step() {
    if (outcome != Outcome::Playing) {
        return;
    }
    tick++;
    conveyor.history.clear(); // Undo is only for planning, not for past movement.
    const Config::LevelTuning& tuning = conveyor.layout.tuning;
    if (tick >= nextSpawnTick && generated < tuning.total) {
        if (spawn()) {
            int delay = tuning.spawnTicks;
            if (rush()) {
                delay = delay / Config::RushSpawnDivisor;
                if (delay < Config::RushMinSpawnTicks) {
                    delay = Config::RushMinSpawnTicks;
                }
            }
            nextSpawnTick = tick + delay;
        }
    }

    dispatch.update(tick, tuning.loaderTicks);
    if (tick % tuning.moveTicks == 0) {
        conveyor.move(dispatch.registry, dispatch, tick);
        conveyor.feed(dispatch.registry, tick);
        int waiting = 0;
        for (int i = 0; i < conveyor.layout.inputCount; i++) {
            waiting += conveyor.inputs[i].size();
        }
        int congestion = conveyor.blocked;
        if (waiting >= Config::InputCapacity) {
            congestion += 2;
        }
        if (congestion >= Config::JamBlockedThreshold) {
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
