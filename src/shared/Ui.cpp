#include "Ui.h"
#include <cmath>  // Angles, distances, rounding and looping motion for the Raylib graphics.
#include <string> // Text messages, number-to-text conversion and font file paths.

namespace {
Font fonts[4]{};
bool loaded[4]{};
Font fontFor(int size, bool bold) {
    int index = size > 36 ? 3 : size > 20 ? 2 : bold ? 1 : 0;
    return loaded[index] ? fonts[index] : GetFontDefault();
}
} // namespace
void Ui::loadFonts() {
    const int sizes[4] = {20, 20, 32, 72};
    for (int i = 0; i < 4; ++i) {
        const char* name = i == 0 ? "Tomorrow-Medium.ttf" : "Tomorrow-SemiBold.ttf";
        // Resolve beside the EXE, so double-click and VS Code use the same font.
        std::string path = std::string(GetApplicationDirectory()) + "assets/fonts/" + name;
        if (!FileExists(path.c_str())) {
            path = std::string("assets/fonts/") + name;
        }
        if (!FileExists(path.c_str())) {
            continue;
        }
        // Load at the displayed size so enlarging the window keeps the letters clear.
        fonts[i] = LoadFontEx(path.c_str(), (int)std::round(sizes[i] * Config::WindowScale), nullptr, 95);
        loaded[i] = fonts[i].texture.id != 0 && fonts[i].texture.id != GetFontDefault().texture.id;
        if (loaded[i]) {
            SetTextureFilter(fonts[i].texture, TEXTURE_FILTER_BILINEAR);
        }
    }
}
void Ui::unloadFonts() {
    for (int i = 0; i < 4; ++i) {
        if (loaded[i]) {
            UnloadFont(fonts[i]);
        }
        loaded[i] = false;
    }
}
bool Ui::fontsReady() {
    return loaded[0] && loaded[1] && loaded[2] && loaded[3];
}
float Ui::textWidth(const char* label, int size, bool bold) {
    return MeasureTextEx(fontFor(size, bold), label, (float)(size), 0).x;
}
void Ui::text(const char* label, float x, float y, int size, Color tint, bool bold) {
    DrawTextEx(fontFor(size, bold), label, {std::round(x), std::round(y)}, (float)(size), 0, tint);
}
void Ui::centered(const char* label, Rectangle bounds, int size, Color tint, bool bold) {
    text(label, bounds.x + (bounds.width - textWidth(label, size, bold)) / 2,
         bounds.y + (bounds.height - size) / 2 - 1, size, tint, bold);
}
void Ui::box(Rectangle bounds, Color tint, float roundness) {
    DrawRectangleRounded(bounds, roundness, 6, tint);
}
void Ui::clip(Rectangle bounds) {
    // Raylib clipping uses window pixels, while drawing uses our original layout coordinates.
    BeginScissorMode((int)(bounds.x * Config::WindowScale), (int)(bounds.y * Config::WindowScale),
                     (int)(bounds.width * Config::WindowScale), (int)(bounds.height * Config::WindowScale));
}
bool Ui::clicked(Rectangle bounds) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), bounds);
}
void Ui::button(const char* label, Rectangle bounds, bool primary) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), bounds);
    box({bounds.x, bounds.y + 3, bounds.width, bounds.height}, Color{9, 16, 23, 255});
    box(bounds, primary ? (hover ? Color{125, 235, 221, 255} : Cyan) : (hover ? Border : Raised));
    centered(label, bounds, 20, primary ? Background : White, true);
}
void Ui::meter(Rectangle bounds, float ratio, Color tint) {
    box(bounds, Background, 0.9f);
    if (ratio < 0) {
        ratio = 0;
    }
    if (ratio > 1) {
        ratio = 1;
    }
    if (ratio > 0) {
        float fillWidth = bounds.width * ratio;
        if (fillWidth < bounds.height) {
            fillWidth = bounds.height;
        }
        box({bounds.x, bounds.y, fillWidth, bounds.height}, tint, 0.9f);
    }
}
void Ui::arrow(Vector2 position, float angle, float size, Color tint) {
    Vector2 direction{std::cos(angle), std::sin(angle)};
    Vector2 tip{position.x + direction.x * size, position.y + direction.y * size};
    Vector2 back{position.x - direction.x * size * 0.7f, position.y - direction.y * size * 0.7f};
    Vector2 left{position.x - direction.y * size * 0.65f, position.y + direction.x * size * 0.65f};
    Vector2 right{position.x + direction.y * size * 0.65f, position.y - direction.x * size * 0.65f};
    float stroke = size < 8 ? 2.0f : 3.0f;
    DrawLineEx(back, tip, stroke, tint);
    DrawLineEx(left, tip, stroke, tint);
    DrawLineEx(right, tip, stroke, tint);
}
Color Ui::destinationColor(Destination destination) {
    switch (destination) {
    case Destination::Red:
        return {242, 113, 116, 255};
    case Destination::Blue:
        return {112, 177, 244, 255};
    case Destination::Yellow:
        return Gold;
    case Destination::Recycle:
        return {113, 211, 153, 255};
    }
    return White;
}
const char* Ui::destinationLetter(Destination destination) {
    return destination == Destination::Red      ? "R"
           : destination == Destination::Blue   ? "B"
           : destination == Destination::Yellow ? "Y"
                                                : "X";
}
void Ui::parcel(Vector2 p, Destination destination, ParcelKind kind, float opacity, float scale) {
    float width = 32 * scale, height = 30 * scale;
    box({p.x - width / 2 + 2, p.y - height / 2 + 5, width, height}, Fade(BLACK, opacity * 0.3f));
    box({p.x - width / 2, p.y - height / 2, width, height}, Fade(Color{188, 141, 88, 255}, opacity));
    box({p.x - width / 2, p.y - height / 2, width, 9 * scale}, Fade(Color{224, 184, 129, 255}, opacity));
    DrawRectangleRec({p.x - 3 * scale, p.y - height / 2, 6 * scale, height},
                     Fade(Color{242, 212, 160, 255}, opacity));
    Rectangle label{p.x - 12 * scale, p.y - 7 * scale, 24 * scale, 19 * scale};
    box(label, Fade(destinationColor(destination), opacity), 0.12f);
    centered(destinationLetter(destination), label, (int)(18 * scale), Fade(Background, opacity), true);
    if (kind == ParcelKind::Fragile || kind == ParcelKind::Express) {
        Rectangle badge{p.x + 7 * scale, p.y - 23 * scale, 17 * scale, 17 * scale};
        box(badge, Fade(kind == ParcelKind::Fragile ? White : Gold, opacity), 0.4f);
        centered(kind == ParcelKind::Fragile ? "F" : "!", badge, (int)(13 * scale), Fade(Background, opacity),
                 true);
    } else if (kind == ParcelKind::Damaged) {
        DrawLineEx({p.x - 13 * scale, p.y - 12 * scale}, {p.x - 8 * scale, p.y - 6 * scale}, 2,
                   Fade(Background, opacity));
        DrawLineEx({p.x - 8 * scale, p.y - 6 * scale}, {p.x - 12 * scale, p.y}, 2, Fade(Background, opacity));
    }
}
Rectangle Ui::levelCard(int index) {
    return {70.0f + index * 295, 245, 275, 320};
}
