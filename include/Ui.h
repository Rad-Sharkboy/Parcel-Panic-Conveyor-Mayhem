#pragma once
#include "raylib.h"
#include "Parcel.h"

// Shared drawing helpers and button rectangles. Input uses the same rectangles.
namespace Ui {
constexpr Color Background{18, 25, 34, 255};
constexpr Color Panel{29, 40, 52, 255};
constexpr Color Raised{40, 54, 68, 255};
constexpr Color Border{61, 78, 93, 255};
constexpr Color White{241, 242, 230, 255};
constexpr Color Muted{165, 183, 196, 255};
constexpr Color Cyan{89, 214, 203, 255};
constexpr Color Gold{247, 194, 91, 255};
constexpr Rectangle Play{82, 340, 320, 56};
constexpr Rectangle Select{82, 410, 320, 50};
constexpr Rectangle Help{82, 474, 320, 50};
constexpr Rectangle Quit{82, 538, 320, 50};
constexpr Rectangle Back{70, 600, 220, 48};
constexpr Rectangle Resume{895, 477, 151, 42};
constexpr Rectangle Restart{1058, 477, 161, 42};
constexpr Rectangle MainMenu{895, 529, 324, 38};
constexpr Rectangle Retry{390, 440, 230, 52};
constexpr Rectangle Next{650, 440, 230, 52};
constexpr Rectangle ResultsMenu{520, 510, 230, 42};
void loadFonts();
void unloadFonts();
bool fontsReady();
void text(const char* label, float x, float y, int size = 20, Color tint = White, bool bold = false);
float textWidth(const char* label, int size, bool bold = false);
void centered(const char* label, Rectangle bounds, int size, Color tint = White, bool bold = false);
void box(Rectangle bounds, Color tint = Panel, float roundness = 0.18f);
void button(const char* label, Rectangle bounds, bool primary = false);
bool clicked(Rectangle bounds);
void meter(Rectangle bounds, float ratio, Color tint);
void arrow(Vector2 position, float angle, float size, Color tint);
Color destinationColor(Destination destination);
const char* destinationLetter(Destination destination);
void parcel(Vector2 position, Destination destination, ParcelKind kind, float opacity = 1.0f, float scale = 1.0f);
Rectangle levelCard(int index);
}
