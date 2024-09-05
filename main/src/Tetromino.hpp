#ifndef TETROMINO_HPP
#define TETROMINO_HPP

#include "engine.hpp"
#include <vector>
#include <array>
#include "config.hpp"

class Tetromino {
public:
    std::vector<std::array<int,2>> shape;
    int x, y;
    SDL_Color color;

    Tetromino(std::vector<std::array<int, 2>> shape) : shape(shape), x(GRID_COLUMNS/2), y(0), color(COLORS[0]) {};

    void move(int, int);
    void draw(SDL_Renderer*);
    bool collisionCheck();
    void rotate();
};

#endif