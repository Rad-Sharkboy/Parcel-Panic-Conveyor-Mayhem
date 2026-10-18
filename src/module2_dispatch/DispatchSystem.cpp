#include "DispatchSystem.h"
int DispatchSystem::multiplier() const {
    int result = 1 + streak / Config::ComboStep;
    if (result > Config::MaxMultiplier) {
        result = Config::MaxMultiplier;
    }
    return result;
}
void DispatchSystem::reset() {
    registry.clear();
    ready.clear();
    nextSequence = 0;
    nextLoadTick = 0;
    score = streak = delivered = failed = 0;
    lastEventTick = -100;
    lastEventId = lastPoints = 0;
}
void DispatchSystem::fail(int id, ParcelStatus reason, int tick) {
    Parcel* parcel = registry.find(id);
    if (!parcel || terminal(parcel->status)) {
        return;
    }
    if (parcel->cell >= 0) {
        parcel->eventCell = parcel->cell;
    }
    parcel->pointsAwarded = -Config::PenaltyPoints;
    parcel->status = reason;
    parcel->cell = -1;
    ++failed;
    streak = 0;
    score -= Config::PenaltyPoints;
    if (score < 0) {
        score = 0;
    }
    lastEventTick = tick;
    lastEventId = id;
    lastPoints = -Config::PenaltyPoints;
}
void DispatchSystem::arrive(int id, Destination dock, int tick) {
    Parcel* parcel = registry.find(id);
    if (!parcel || parcel->status != ParcelStatus::OnConveyor) {
        return;
    }
    parcel->eventCell = parcel->cell;
    parcel->status = ParcelStatus::AtDock;
    parcel->cell = -1;
    if (parcel->destination != dock) {
        fail(id, ParcelStatus::Rejected, tick);
        return;
    }
    parcel->status = ParcelStatus::Ready;
    ready.insert({id, parcel->deadline, nextSequence++});
}
void DispatchSystem::update(int tick, int interval) {
    for (int i = 0; i < registry.getCapacity(); ++i) {
        const Bucket& bucket = registry.getBucket(i);
        if (bucket.state == BucketState::Occupied && !terminal(bucket.parcel.status) &&
            tick > bucket.parcel.deadline) {
            fail(bucket.parcel.id, ParcelStatus::Expired, tick);
        }
    }
    if (tick < nextLoadTick) {
        return;
    }
    HeapEntry entry;
    while (ready.extract(entry)) {
        Parcel* parcel = registry.find(entry.id);
        if (!parcel || parcel->status != ParcelStatus::Ready) {
            continue;
        }
        parcel->status = ParcelStatus::Dispatched;
        ++delivered;
        ++streak;
        lastPoints =
            (parcel->kind == ParcelKind::Damaged ? Config::RecyclePoints : Config::BasePoints) * multiplier();
        parcel->pointsAwarded = lastPoints;
        score += lastPoints;
        lastEventTick = tick;
        lastEventId = parcel->id;
        nextLoadTick = tick + interval;
        // An express dispatch gives queued/active parcels a small deadline extension.
        if (parcel->kind == ParcelKind::Express) {
            for (int i = 0; i < registry.getCapacity(); ++i) {
                const Bucket& bucket = registry.getBucket(i);
                if (bucket.state == BucketState::Occupied &&
                    (bucket.parcel.status == ParcelStatus::Queued ||
                     bucket.parcel.status == ParcelStatus::OnConveyor)) {
                    registry.find(bucket.parcel.id)->deadline += Config::ExpressBonusTicks;
                }
            }
        }
        break;
    }
}
