#include <vector>
#include <array>
#include <iostream>
#include "engine.hpp"
#include "config.hpp"
#include <ctime>

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

void Tetromino::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, this->color.r, this->color.g, this->color.b, this->color.a);
    for (auto block: shape) {
        SDL_Rect square = { x * CELL_SIZE + block[0] * CELL_SIZE, y * CELL_SIZE + block[1] * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &square);
    }
}

bool Tetromino::collisionCheck() {

    for (auto cubeMoving: shape) {
        if (cubeMoving[1] + y >= GRID_ROWS) return true; // bottom grid
        else if (cubeMoving[0] + x >= GRID_COLUMNS || cubeMoving[0] + x <= -1) return true; //sides grid

        for (int i = 0; i < GRID_COLUMNS; ++i) {
            for (int j = 0; j < GRID_ROWS; ++j) {
                if (gridBlocks[i][j]) {
                    if (cubeMoving[0] + x == i && cubeMoving[1] + y == j) return true;
                }
            }
        }
    }
    
    return false;
}

void spawnTetromino(Tetromino* tetromino) {

    for (auto block: tetromino->shape) {
        gridBlocks[tetromino->x + block[0]][tetromino->y + block [1]] = true;
    }

    std::srand(std::time(nullptr));
    int rand = std::rand();

    tetromino->shape = SHAPES[rand % 7];
    tetromino->x = GRID_COLUMNS / 2;
    tetromino->y = 0;
    tetromino->color = COLORS[rand % 7];

    if (tetromino->collisionCheck()) gameOver = true;

}

void Tetromino::move(int dx, int dy) {

    int ogX = x;
    int ogY = y;
    
    x += dx;
    y += dy;

    switch (collisionCheck()) {
    case false:
        std::cout<<"none "<<dy<<dx;
        break;
    case true:
        if (dx != 0) {
            std::cout<<"sides ";
            x = ogX;
            y = ogY;
        }
        else {
            std::cout<<"bottom ";
            x = ogX;
            y = ogY;
            spawnTetromino(this);
        }
    };

}

void Tetromino::rotate() {
    std::vector<std::array<int, 2>> rotatedShape;

    // rotate
    for (const auto& block : shape) {
        rotatedShape.push_back({ block[1], -block[0] });
    }

    // standartise
    int minX = rotatedShape[0][0];
    int minY = rotatedShape[0][1];

    for (const auto& block : rotatedShape) {
        if (block[0] < minX) minX = block[0];
        if (block[1] < minY) minY = block[1];
    }

    for (auto& block : rotatedShape) {
        block[0] -= minX;
        block[1] -= minY;
    }

    
    std::vector<std::array<int,2>> shapeCopy = shape;
    shape = rotatedShape;

    if (collisionCheck()) shape = shapeCopy;
    
}