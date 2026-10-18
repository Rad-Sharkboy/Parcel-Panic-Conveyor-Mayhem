#pragma once
#include "Game.h"
class Renderer {
public:
    static void draw(const Game& game);
    static Vector2 center(int cell);
private:
    static void background(const Game& game);
    static void factory(const Game& game);
    static void dashboard(const Game& game);
    static void menu(const Game& game);
    static void demonstration(const Game& game);
    static void results(const Game& game);
};
