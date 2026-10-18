#include "Game.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cerrno>
#include <climits>
namespace {
bool parseNumber(const char* input, int& value) {
    char* end = nullptr; errno = 0;
    long parsed = std::strtol(input, &end, 10);
    if (errno || end == input || *end != '\0' || parsed < 1 || parsed > INT_MAX) return false;
    value = static_cast<int>(parsed); return true;
}
}
int main(int argc, char** argv) {
    int frames = 0, level = 0; const char* screenshot = nullptr; const char* stateFile = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--smoke") == 0 && i + 1 < argc) { if (!parseNumber(argv[++i], frames)) { std::cerr << "Smoke frame count must be a positive integer\n"; return 2; } }
        else if (std::strcmp(argv[i], "--level") == 0 && i + 1 < argc) { if (!parseNumber(argv[++i], level)) { std::cerr << "Level must be 1..4\n"; return 2; } --level; }
        else if (std::strcmp(argv[i], "--screenshot") == 0 && i + 1 < argc) screenshot = argv[++i];
        else if (std::strcmp(argv[i], "--state-file") == 0 && i + 1 < argc) stateFile = argv[++i];
        else { std::cerr << "Usage: parcel_panic [--level 1..4] [--smoke FRAMES] [--screenshot path.png] [--state-file path.json]\n"; return 2; }
    }
    if (screenshot && frames == 0) { std::cerr << "--screenshot requires --smoke FRAMES\n"; return 2; }
    if (level < 0 || level >= Config::Levels) { std::cerr << "Level must be 1..4\n"; return 2; }
    Game game; game.selectedLevel = level; return game.run(frames, screenshot, stateFile) ? 0 : 1;
}
