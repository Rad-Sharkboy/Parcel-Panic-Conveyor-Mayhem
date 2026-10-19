// === TUNING & CONFIGURATION CONSTANTS ===
#ifndef CONFIG_H
#define CONFIG_H // Prevent this header being included twice in the same source file.
// Config:: groups the game settings under one name. const means the value stays fixed.
namespace Config {
const int WindowWidth = 1280;
const int WindowHeight = 720;
// Keep the layout coordinates above; draw them 6.25% larger in a 1360 x 765 window.
const float WindowScale = 1.0625f;
const int RenderFPS = 60;
const float MaxFrameDelta = 0.1f;
const int Columns = 17;
const int Rows = 11;
const int Cells = Columns * Rows;
const int Tile = 48;
const int BoardX = 28;
const int BoardY = 140;
const int InputCapacity = 8;
const int HeapCapacity = 8;
const int HashCapacity = 17;
const int MaxInputs = 4;
const int MaxJunctions = 10;
const int Levels = 4;
const float TickSeconds = 0.22f;
const int BrakeTicks = 12;
const int BrakeCooldown = 32;
const int DeadlineTicks = 150;
const int ExpressDeadlineTicks = 85;
const int ExpressBonusTicks = 8;
const int BasePoints = 100;
const int RecyclePoints = 55;
const int PenaltyPoints = 40;
const int ComboStep = 3;
const int MaxMultiplier = 5;
const int JamLimit = 100;
const int JamRise = 2;
const int JamBlockedThreshold = 4;
const int JamRecovery = 3;
const int RushPeriod = 100;
const int RushDuration = 25;
const int RushWarning = 15;
const int RushSpawnDivisor = 2;
const int RushMinSpawnTicks = 2;
const int DamagedPeriod = 11;
const int DamagedOffset = 5;
const int FragilePeriod = 5;
const int FragileOffset = 2;
const int ExpressPeriod = 4;
const int ExpressOffset = 1;
const int MaxLevelTicks = 1800;
const int DemoCapacity = 6;
const int ParticleCount = 80;
const int PopupCount = 16;
const float ParcelTravelFraction = 0.82f;
const float ParcelFadeSeconds = 0.24f;
const float PopupSeconds = 1.2f;
const float ParticleSeconds = 0.6f;
const float JunctionTurnSpeed = 10.0f;
const float LevelTitleSeconds = 2.5f;
const float BeltPixelsPerSecond = 22.0f;
const float BeltRibSpacing = 12.0f;
struct LevelTuning {
    int total, quota, target, spawnTicks, moveTicks, loaderTicks, colors;
    bool specials, rush;
};
const int Level1Parcels = 20;
const int Level2Parcels = 38;
const int Level3Parcels = 48;
const int Level4Parcels = 60;
const LevelTuning LevelsTuning[Levels] = {{Level1Parcels, 12, 1200, 12, 3, 4, 2, false, false},
                                          {Level2Parcels, 23, 2300, 8, 2, 5, 3, false, true},
                                          {Level3Parcels, 29, 2800, 8, 2, 6, 3, true, true},
                                          {Level4Parcels, 36, 3600, 6, 2, 8, 3, true, true}};
// Level 4 has the most parcels; use its total for the visual arrays too.
const int MaxParcels = Level4Parcels;
} // namespace Config

#endif
