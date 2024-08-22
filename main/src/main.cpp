#include <SDL.h>
#include <iostream>
#include <vector>
#include <array>
#include "engine.hpp"

const int GRID_COLUMNS = 10;
const int GRID_ROWS = 16;
const int CELL_SIZE = 30; 
const int SCREEN_WIDTH = GRID_COLUMNS * CELL_SIZE;
const int SCREEN_HEIGHT = GRID_ROWS * CELL_SIZE;
const int GAME_SPEED = 100;

const std::vector<std::vector<std::array<int, 2>>> SHAPES = {
    { {0,0}, {0,1}, {0,2}, {0,3}  },
    { {0,0}, {0,1}, {1,0}, {1,1}  }
};


enum CollisionSide {
    NONE,
    SIDES,
    BOTTOM
};

class Tetromino {
public:
    std::vector<std::array<int,2>> shape;
    int x, y;

    Tetromino(std::vector<std::array<int, 2>> shape) : shape(shape), x(GRID_COLUMNS/2), y(0) {};

    void move(int, int);
    void draw(SDL_Renderer*);
    CollisionSide collisionCheck();
};

void Tetromino::draw(SDL_Renderer* renderer) {
    for (auto block: shape) {
        SDL_Rect square = { x * CELL_SIZE + block[0] * CELL_SIZE, y * CELL_SIZE + block[1] * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &square);
    }
}

void Tetromino::move(int dx, int dy) {

    switch (collisionCheck()) {
    case NONE:
        break;
    case SIDES:
        dx = 0;
    case BOTTOM:
        dy = 0;
    };

    x += dx;
    y += dy;
}

CollisionSide Tetromino::collisionCheck() {
    
    return NONE; //next TODO
}

void spawnTetramino(std::vector<Tetromino>* tetrominos) {
    tetrominos->push_back(Tetromino(SHAPES[0]));
}

void drawGrid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    
    for (int i = 0; i <= GRID_COLUMNS; ++i) {
        int x = i * CELL_SIZE;
        SDL_RenderDrawLine(renderer, x, 0, x, GRID_ROWS * CELL_SIZE);
    }
    
    for (int i = 0; i <= GRID_ROWS; ++i) {
        int y = i * CELL_SIZE;
        SDL_RenderDrawLine(renderer, 0, y, GRID_COLUMNS * CELL_SIZE, y);
    }
}

int main(int argc, char* argv[]) {

    Engine *engine = new Engine();

    if (!engine->init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cerr << "Failed to initialize SDL." << std::endl;
        return -1;
    }

    std::vector<Tetromino> tetrominos;
    spawnTetramino(&tetrominos);

    bool quit = false;
    SDL_Event e;

    // Game loop
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {

            switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case SDLK_a:
                    tetrominos.back().move(-1,0);
                    break;
                case SDLK_s:
                    tetrominos.back().move(0,1);
                    break;
                case SDLK_d:
                    tetrominos.back().move(1,0);
                    break;
                }
            }

        }

        SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(engine->renderer);

        drawGrid(engine->renderer);

        for (auto& tetro: tetrominos) {
            tetro.draw(engine->renderer);
            tetro.move(0,1);
        }
        SDL_Delay(GAME_SPEED);

        SDL_RenderPresent(engine->renderer);
    }

    engine->~Engine();

    return 0;
}
