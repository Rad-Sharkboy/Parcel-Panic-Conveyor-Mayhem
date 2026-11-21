#include "DispatchSystem.h"

int DispatchSystem::multiplier() const {
    int comboMultiplier = 1 + streak / Config::ComboStep;
    if (comboMultiplier > Config::MaxMultiplier) {
        comboMultiplier = Config::MaxMultiplier;
    }
    return comboMultiplier;
}

void DispatchSystem::reset() {
    registry.clear();
    ready.clear();
    activity.clear();
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

    // Reject parcel if routed to a mismatched destination dock.
    if (parcel->destination != dock) {
        fail(id, ParcelStatus::Rejected, tick);
        return;
    }
    parcel->status = ParcelStatus::Ready;
    ready.insert({id, parcel->deadline, nextSequence++});
    activity.add(Operation::HeapInsert, id, parcel->deadline);
}

void DispatchSystem::scanExpiredParcels(int tick) {
    int tableCapacity = registry.getCapacity();
    for (int i = 0; i < tableCapacity; ++i) {
        const Bucket& bucket = registry.getBucket(i);
        if (bucket.state == BucketState::Occupied &&
            !terminal(bucket.parcel.status) &&
            tick > bucket.parcel.deadline) {
            fail(bucket.parcel.id, ParcelStatus::Expired, tick);
        }
    }
}

void DispatchSystem::applyExpressDeadlineBonus() {
    int tableCapacity = registry.getCapacity();
    for (int i = 0; i < tableCapacity; ++i) {
        const Bucket& bucket = registry.getBucket(i);
        if (bucket.state == BucketState::Occupied &&
            (bucket.parcel.status == ParcelStatus::Queued ||
             bucket.parcel.status == ParcelStatus::OnConveyor)) {
            Parcel* activeParcel = registry.find(bucket.parcel.id);
            if (activeParcel != nullptr) {
                activeParcel->deadline += Config::ExpressBonusTicks;
            }
        }
    }
}

void DispatchSystem::dispatchNextReadyParcel(int tick, int interval) {
    if (interval < 1) {
        interval = 1;
    }
    HeapEntry topEntry;
    while (ready.extract(topEntry)) {
        Parcel* parcel = registry.find(topEntry.id);
        // Lazy deletion check: skip stale heap entries that are no longer Ready.
        if (!parcel || parcel->status != ParcelStatus::Ready) {
            activity.add(Operation::SkipReady, topEntry.id);
            continue;
        }
        parcel->status = ParcelStatus::Dispatched;
        activity.add(Operation::Dispatch, topEntry.id, topEntry.deadline);
        ++delivered;
        ++streak;

        int baseReward = (parcel->kind == ParcelKind::Damaged)
                             ? Config::RecyclePoints
                             : Config::BasePoints;
        lastPoints = baseReward * multiplier();
        parcel->pointsAwarded = lastPoints;
        score += lastPoints;
        lastEventTick = tick;
        lastEventId = parcel->id;
        nextLoadTick = tick + interval;

        // Express dispatches grant a deadline extension to active and queued parcels.
        if (parcel->kind == ParcelKind::Express) {
            applyExpressDeadlineBonus();
        }
        break;
    }
}

void DispatchSystem::update(int tick, int interval) {
    scanExpiredParcels(tick);
    if (tick < nextLoadTick) {
        return;
    }
    dispatchNextReadyParcel(tick, interval);
}