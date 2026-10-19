#ifndef UI_H
#define UI_H // Prevent this header being included twice in the same source file.
#include "raylib.h"
#include "Parcel.h"

// Shared drawing helpers and button rectangles. Input uses the same rectangles.
namespace Ui {
const Color Background{18, 25, 34, 255};
const Color Panel{29, 40, 52, 255};
const Color Raised{40, 54, 68, 255};
const Color Border{61, 78, 93, 255};
const Color White{241, 242, 230, 255};
const Color Muted{165, 183, 196, 255};
const Color Cyan{89, 214, 203, 255};
const Color Gold{247, 194, 91, 255};
const Rectangle Play{82, 340, 320, 56};
const Rectangle Select{82, 410, 320, 50};
const Rectangle Help{82, 474, 320, 50};
const Rectangle Quit{82, 538, 320, 50};
const Rectangle Back{70, 600, 220, 48};
const Rectangle Resume{895, 477, 151, 42};
const Rectangle Restart{1058, 477, 161, 42};
const Rectangle MainMenu{895, 529, 324, 38};
const Rectangle Retry{390, 440, 230, 52};
const Rectangle Next{650, 440, 230, 52};
const Rectangle ResultsMenu{520, 510, 230, 42};
const Rectangle DemoNext{90, 145, 230, 37};
const Rectangle DemoReset{333, 145, 210, 37};
const Rectangle DemoClose{556, 145, 160, 37};
const Rectangle QueueLane{895, 262, 324, 29};
const Rectangle DemoTabs[3] = {{838, 686, 86, 28}, {930, 686, 86, 28}, {1022, 686, 86, 28}};
const Rectangle InfoToggle{1114, 686, 126, 28};
const Rectangle ParcelInfo{895, 266, 324, 201};
void loadFonts();
void unloadFonts();
bool fontsReady();
void text(const char* label, float x, float y, int size = 20, Color tint = White, bool bold = false);
float textWidth(const char* label, int size, bool bold = false);
void centered(const char* label, Rectangle bounds, int size, Color tint = White, bool bold = false);
void box(Rectangle bounds, Color tint = Panel, float roundness = 0.18f);
void clip(Rectangle bounds);
void button(const char* label, Rectangle bounds, bool primary = false);
bool clicked(Rectangle bounds);
void meter(Rectangle bounds, float ratio, Color tint);
void arrow(Vector2 position, float angle, float size, Color tint);
Color destinationColor(Destination destination);
const char* destinationLetter(Destination destination);
void parcel(Vector2 position, Destination destination, ParcelKind kind, float opacity = 1.0f,
            float scale = 1.0f);
Rectangle levelCard(int index);
} // namespace Ui

#endif
