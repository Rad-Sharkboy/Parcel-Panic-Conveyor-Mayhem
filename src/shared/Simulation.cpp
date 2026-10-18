#include "Simulation.h"
#include <algorithm>
void Simulation::load(int index) {
    level = (index % Config::Levels + Config::Levels) % Config::Levels;
    conveyor.reset(level); dispatch.reset(); tick = generated = jam = nextSpawn_ = 0; outcome = Outcome::Playing;
}
void Simulation::next(int delta) { load(level + delta); }
bool Simulation::rush() const { return conveyor.layout.tuning.rush && tick % Config::RushPeriod >= Config::RushPeriod - Config::RushDuration; }
bool Simulation::rushWarning() const {
    int phase = tick % Config::RushPeriod;
    return conveyor.layout.tuning.rush && phase >= Config::RushPeriod - Config::RushDuration - Config::RushWarning && !rush();
}
bool Simulation::spawn() {
    const Config::LevelTuning& t = conveyor.layout.tuning;
    if (generated >= t.total) return false;
    int lane = generated % conveyor.layout.inputCount;
    if (conveyor.inputs[lane].size() == conveyor.inputs[lane].capacity()) return false;
    Parcel p; p.id = generated + 1; p.destination = static_cast<Destination>(generated % t.colors);
    if (t.specials) {
        if (generated % Config::DamagedPeriod == Config::DamagedOffset) { p.kind = ParcelKind::Damaged; p.destination = Destination::Recycle; }
        else if (generated % Config::FragilePeriod == Config::FragileOffset) p.kind = ParcelKind::Fragile;
        else if (generated % Config::ExpressPeriod == Config::ExpressOffset) p.kind = ParcelKind::Express;
    }
    p.deadline = tick + (p.kind == ParcelKind::Express ? Config::ExpressDeadlineTicks : Config::DeadlineTicks);
    dispatch.registry.insert(p); p.status = ParcelStatus::Queued; dispatch.registry.insert(p);
    conveyor.inputs[lane].enqueue(p.id); ++generated; return true;
}
void Simulation::step() {
    if (outcome != Outcome::Playing) return;
    ++tick; conveyor.history.clear(); const Config::LevelTuning& t = conveyor.layout.tuning;
    if (tick >= nextSpawn_ && generated < t.total) {
        if (spawn()) nextSpawn_ = tick + (rush() ? std::max(Config::RushMinSpawnTicks, t.spawnTicks / Config::RushSpawnDivisor) : t.spawnTicks);
    }
    dispatch.update(tick, t.loaderTicks);
    if (tick % t.moveTicks == 0) {
        conveyor.move(dispatch.registry, dispatch, tick);
        conveyor.feed(dispatch.registry, tick);
        int queued = 0; for (int i = 0; i < conveyor.layout.inputCount; ++i) queued += conveyor.inputs[i].size();
        int congestion = conveyor.blocked + (queued >= Config::InputCapacity ? 2 : 0);
        jam = std::clamp(jam + (congestion >= Config::JamBlockedThreshold ? Config::JamRise : -Config::JamRecovery), 0, Config::JamLimit);
    }
    if (jam >= Config::JamLimit || tick >= Config::MaxLevelTicks) outcome = Outcome::Lost;
    else if (generated == t.total && dispatch.delivered + dispatch.failed == generated) {
        outcome = dispatch.delivered >= t.quota && dispatch.score >= t.target ? Outcome::Won : Outcome::Lost;
    }
}
bool Simulation::validate() const {
    int seen[Config::MaxParcels + 1]{};
    for (int lane = 0; lane < conveyor.layout.inputCount; ++lane) {
        const CircularQueue& q = conveyor.inputs[lane];
        for (int position = 0; position < q.size(); ++position) {
            int id = q.slot((q.front() + position) % q.capacity());
            const Parcel* p = dispatch.registry.find(id);
            if (!p || id < 1 || id > Config::MaxParcels) return false;
            if (terminal(p->status)) continue;
            if (p->status != ParcelStatus::Queued || ++seen[id] != 1) return false;
        }
    }
    for (int i = 0; i < Config::Cells; ++i) {
        int id = conveyor.layout.cells[i].parcel;
        if (!id) continue;
        const Parcel* p = dispatch.registry.find(id);
        if (!p || id < 1 || id > Config::MaxParcels) return false;
        // Terminal IDs may remain until the next cleanup pass, but are never active.
        if (terminal(p->status)) continue;
        if (++seen[id] != 1 || p->status != ParcelStatus::OnConveyor || p->cell != i) return false;
    }
    for (int i = 0; i < dispatch.ready.size(); ++i) {
        int id = dispatch.ready.at(i).id; const Parcel* p = dispatch.registry.find(id);
        if (!p || id < 1 || id > Config::MaxParcels) return false;
        if (terminal(p->status)) continue;
        if (p->status != ParcelStatus::Ready || ++seen[id] != 1) return false;
    }
    int delivered = 0, failed = 0;
    for (int i = 0; i < dispatch.registry.capacity(); ++i) {
        const Bucket& b = dispatch.registry.bucket(i);
        if (b.state != BucketState::Occupied) continue;
        if (b.parcel.id < 1 || b.parcel.id > Config::MaxParcels) return false;
        if (b.parcel.status == ParcelStatus::Dispatched) ++delivered;
        else if (terminal(b.parcel.status)) ++failed;
        else if (seen[b.parcel.id] != 1) return false;
    }
    return delivered == dispatch.delivered && failed == dispatch.failed && dispatch.registry.size() == generated;
}
