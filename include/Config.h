// === TUNING & CONFIGURATION CONSTANTS ===
#pragma once
namespace Config {
constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr int RenderFPS = 60;
constexpr float MaxFrameDelta = 0.1f;
constexpr int Columns = 17;
constexpr int Rows = 11;
constexpr int Cells = Columns * Rows;
constexpr int Tile = 48;
constexpr int BoardX = 28;
constexpr int BoardY = 140;
constexpr int InputCapacity = 8;
constexpr int HeapCapacity = 8;
constexpr int HashCapacity = 17;
constexpr int MaxInputs = 4;
constexpr int MaxJunctions = 10;
constexpr int Levels = 4;
constexpr float TickSeconds = 0.22f;
constexpr int BrakeTicks = 12;
constexpr int BrakeCooldown = 32;
constexpr int DeadlineTicks = 150;
constexpr int ExpressDeadlineTicks = 85;
constexpr int ExpressBonusTicks = 8;
constexpr int BasePoints = 100;
constexpr int RecyclePoints = 55;
constexpr int PenaltyPoints = 40;
constexpr int ComboStep = 3;
constexpr int MaxMultiplier = 5;
constexpr int JamLimit = 100;
constexpr int JamRise = 2;
constexpr int JamBlockedThreshold = 4;
constexpr int JamRecovery = 3;
constexpr int RushPeriod = 100;
constexpr int RushDuration = 25;
constexpr int RushWarning = 15;
constexpr int RushSpawnDivisor = 2;
constexpr int RushMinSpawnTicks = 2;
constexpr int DamagedPeriod = 11;
constexpr int DamagedOffset = 5;
constexpr int FragilePeriod = 5;
constexpr int FragileOffset = 2;
constexpr int ExpressPeriod = 4;
constexpr int ExpressOffset = 1;
constexpr int MaxLevelTicks = 1800;
constexpr int DemoCapacity = 6;
constexpr int ParticleCount = 80;
constexpr int PopupCount = 16;
constexpr float ParcelTravelFraction = 0.82f;
constexpr float ParcelFadeSeconds = 0.24f;
constexpr float PopupSeconds = 1.2f;
constexpr float ParticleSeconds = 0.6f;
constexpr float JunctionTurnSpeed = 10.0f;
constexpr float LevelTitleSeconds = 2.5f;
constexpr float BeltPixelsPerSecond = 22.0f;
constexpr float BeltRibSpacing = 12.0f;
struct LevelTuning { int total, quota, target, spawnTicks, moveTicks, loaderTicks, colors; bool specials, rush; };
constexpr LevelTuning LevelsTuning[Levels] = {
    {20, 12, 1200, 12, 3, 4, 2, false, false},
    {38, 23, 2300, 8, 2, 5, 3, false, true},
    {48, 29, 2800, 8, 2, 6, 3, true, true},
    {60, 36, 3600, 6, 2, 8, 3, true, true}
};
constexpr int largestLevel() {
    int maximum = 0;
    for (int i = 0; i < Levels; ++i) if (LevelsTuning[i].total > maximum) maximum = LevelsTuning[i].total;
    return maximum;
}
constexpr int MaxParcels = largestLevel();
static_assert(TickSeconds > 0 && InputCapacity > 0 && HeapCapacity > 0 && HashCapacity > 0, "Capacities and tick duration must be positive");
static_assert(DamagedPeriod > 0 && FragilePeriod > 0 && ExpressPeriod > 0 && RushSpawnDivisor > 0, "Periods must be positive");
}
