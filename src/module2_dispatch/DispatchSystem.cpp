#include "DispatchSystem.h"
#include <algorithm>
int DispatchSystem::multiplier() const { return std::min(Config::MaxMultiplier, 1 + streak / Config::ComboStep); }
void DispatchSystem::reset() {
    registry.clear(); ready.clear(); sequence_ = 0; nextLoad_ = 0;
    score = streak = delivered = failed = 0; lastEventTick = -100; lastEventId = lastPoints = 0;
}
void DispatchSystem::fail(int id, ParcelStatus reason, int tick) {
    Parcel* p = registry.find(id);
    if (!p || terminal(p->status)) return;
    if (p->cell >= 0) p->eventCell = p->cell;
    p->pointsAwarded = -Config::PenaltyPoints;
    p->status = reason; p->cell = -1; ++failed; streak = 0;
    score = std::max(0, score - Config::PenaltyPoints);
    lastEventTick = tick; lastEventId = id; lastPoints = -Config::PenaltyPoints;
}
void DispatchSystem::arrive(int id, Destination dock, int tick) {
    Parcel* p = registry.find(id);
    if (!p || p->status != ParcelStatus::OnConveyor) return;
    p->eventCell = p->cell;
    p->status = ParcelStatus::AtDock; p->cell = -1;
    if (p->destination != dock) { fail(id, ParcelStatus::Rejected, tick); return; }
    p->status = ParcelStatus::Ready; ready.insert({id, p->deadline, sequence_++});
}
void DispatchSystem::update(int tick, int interval) {
    for (int i = 0; i < registry.capacity(); ++i) {
        const Bucket& b = registry.bucket(i);
        if (b.state == BucketState::Occupied && !terminal(b.parcel.status) && tick > b.parcel.deadline) fail(b.parcel.id, ParcelStatus::Expired, tick);
    }
    if (tick < nextLoad_) return;
    HeapEntry entry;
    while (ready.extract(entry)) {
        Parcel* p = registry.find(entry.id);
        if (!p || p->status != ParcelStatus::Ready) continue;
        p->status = ParcelStatus::Dispatched; ++delivered; ++streak;
        lastPoints = (p->kind == ParcelKind::Damaged ? Config::RecyclePoints : Config::BasePoints) * multiplier();
        p->pointsAwarded = lastPoints;
        score += lastPoints; lastEventTick = tick; lastEventId = p->id;
        nextLoad_ = tick + interval;
        // An express dispatch gives queued/active parcels a small deadline extension.
        if (p->kind == ParcelKind::Express) {
            for (int i = 0; i < registry.capacity(); ++i) {
                const Bucket& b = registry.bucket(i);
                if (b.state == BucketState::Occupied && (b.parcel.status == ParcelStatus::Queued || b.parcel.status == ParcelStatus::OnConveyor)) registry.find(b.parcel.id)->deadline += Config::ExpressBonusTicks;
            }
        }
        break;
    }
}
