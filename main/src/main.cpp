#include <SDL.h>
#include <iostream>
#include <vector>
#include <array>
#include <windows.h>
#include "engine.hpp"

const int GRID_COLUMNS = 10;
const int GRID_ROWS = 16;
const int CELL_SIZE = 30; 
const int SCREEN_WIDTH = GRID_COLUMNS * CELL_SIZE;
const int SCREEN_HEIGHT = GRID_ROWS * CELL_SIZE;
const int GAME_SPEED = 200;

const std::vector<std::vector<std::array<int, 2>>> SHAPES = {
    { {0,0}, {0,1}, {0,2}, {0,3}  },
    { {0,0}, {0,1}, {1,0}, {1,1}  }
};


// enum CollisionSide {
//     NONE,
//     SIDES,
//     BOTTOM
// };

class Tetromino {
public:
    std::vector<std::array<int,2>> shape;
    int x, y;

    Tetromino(std::vector<std::array<int, 2>> shape) : shape(shape), x(GRID_COLUMNS/2), y(0) {};

    void move(int, int, std::vector<Tetromino>* tetrominos);
    void draw(SDL_Renderer*);
    bool collisionCheck(std::vector<Tetromino> tetrominos);
};

void Tetromino::draw(SDL_Renderer* renderer) {
    for (auto block: shape) {
        SDL_Rect square = { x * CELL_SIZE + block[0] * CELL_SIZE, y * CELL_SIZE + block[1] * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &square);
    }
}

void spawnTetromino(std::vector<Tetromino>* tetrominos) {
    tetrominos->push_back(Tetromino(SHAPES[0]));
}

void Tetromino::move(int dx, int dy, std::vector<Tetromino>* tetrominos) {

    int ogX = x;
    int ogY = y;
    
    x += dx;
    y += dy;

    switch (collisionCheck(*tetrominos)) {
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
            spawnTetromino(tetrominos);
        }
    };

}

bool Tetromino::collisionCheck(std::vector<Tetromino> tetrominos) {

    for (auto cubeMoving: shape) {
        if (cubeMoving[1] + y >= GRID_ROWS) return true; // bottom grid
        else if (cubeMoving[0] + x >= GRID_COLUMNS || cubeMoving[0] + x <= -1) return true; //sides grid

        for (size_t i = 0; i < tetrominos.size() - 1; ++i) {
            Tetromino& tetroSitting = tetrominos[i];

            for (auto cubeSitting: tetroSitting.shape) {
                if (cubeMoving[0] + x == cubeSitting[0] + tetroSitting.x && cubeMoving[1] + y == cubeSitting[1] + tetroSitting.y) return true;
            }
        }

    }

    /////////////
    
    // if (tetrominos.size() == 1) {
    //     for (auto cubeMoving: tetrominos.back().shape) {
    //         if (cubeMoving[1] + tetrominos.back().y == GRID_ROWS - 1) return BOTTOM;
    //     }
    // }
    // else {
    //     // each tetromino exept last
    //     for (size_t i = 0; i <= tetrominos.size() - 2; ++i) {
    //         // each siting cube
    //         for (auto cubeSitting: tetrominos[i].shape) {
    //             // each moving cube
    //             for (auto cubeMoving: tetrominos.back().shape) {
                    
    //                 if (cubeMoving[1] + tetrominos.back().y == GRID_ROWS - 1) return BOTTOM;
    //                 //else if (cubeSitting[1] + tetrominos[i].y == cubeMoving[1] + tetrominos.back().y) return BOTTOM;
    //                 else if (cubeSitting[0] + tetrominos[i].x == cubeMoving[0] + tetrominos.back().x &&
    //                     cubeSitting[1] + tetrominos[i].y == cubeMoving[1] + tetrominos.back().y)  return SIDES; //temp TODO
    //                     //kadangi nepermetam dx reiksmes nezinom i kuria puse juda tetro del to nezinom ar bus soninis colision TODO
    //             }
    //         }
    //     }
    // }
    
    return false;
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

void refresh(Engine *engine, std::vector<Tetromino> tetrominos) {
    SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(engine->renderer);

    drawGrid(engine->renderer);

    for (auto& tetro: tetrominos) {
        tetro.draw(engine->renderer); 
    }

    SDL_RenderPresent(engine->renderer);
}

int main(int argc, char* argv[]) {

    AllocConsole();

    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    std::cout << "Debug console" << std::endl;

    Engine *engine = new Engine();

    if (!engine->init(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        std::cerr << "Failed to initialize SDL." << std::endl;
        return -1;
    }

    std::vector<Tetromino> tetrominos;
    spawnTetromino(&tetrominos);

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
                    tetrominos.back().move(-1,0, &tetrominos);
                    refresh(engine, tetrominos);
                    break;
                case SDLK_s:
                    tetrominos.back().move(0,1, &tetrominos);
                    refresh(engine, tetrominos);
                    break;
                case SDLK_d:
                    tetrominos.back().move(1,0, &tetrominos);
                    refresh(engine, tetrominos);
                    break;
                }
            }

        }

        refresh(engine, tetrominos);

        tetrominos.back().move(0,1, &tetrominos);

        SDL_Delay(GAME_SPEED);

        //
    }

    engine->~Engine();

    return 0;
}
