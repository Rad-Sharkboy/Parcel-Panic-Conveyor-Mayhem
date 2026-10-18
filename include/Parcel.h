#pragma once
#include "Config.h"
enum class Destination { Red, Blue, Yellow, Recycle };
enum class ParcelKind { Normal, Express, Fragile, Damaged };
enum class ParcelStatus { Created, Queued, OnConveyor, AtDock, Ready, Dispatched, Rejected, Broken, Expired };
struct Parcel {
    int id = 0;
    Destination destination = Destination::Red;
    ParcelKind kind = ParcelKind::Normal;
    ParcelStatus status = ParcelStatus::Created;
    int deadline = 0;
    int cell = -1;
    int previousCell = -1;
    int movedTick = 0;
    // Keep the actual outcome location/reward after cell is cleared.
    int eventCell = -1;
    int pointsAwarded = 0;
};
inline bool terminal(ParcelStatus s) {
    return s == ParcelStatus::Dispatched || s == ParcelStatus::Rejected || s == ParcelStatus::Broken || s == ParcelStatus::Expired;
}
