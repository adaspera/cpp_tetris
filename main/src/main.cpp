#include <SDL.h>
#include <iostream>
#include <vector>
#include <array>
#include <windows.h>
#include <cstring>
#include "engine.hpp"

const int GRID_COLUMNS = 10;
const int GRID_ROWS = 16;
const int CELL_SIZE = 30; 
const int SCREEN_WIDTH = GRID_COLUMNS * CELL_SIZE;
const int SCREEN_HEIGHT = GRID_ROWS * CELL_SIZE;
const int GAME_SPEED = 200;

const std::vector<std::vector<std::array<int, 2>>> SHAPES = {
    { {0,0}, {0,1}, {0,2}, {0,3} }, // I
    { {0,0}, {0,1}, {1,0}, {1,1} }, // block
    { {0,0}, {1,0}, {1,1}, {2,1} }, // Z-1
    { {1,0}, {2,0}, {0,1}, {1,1} }, // Z-2
    { {0,0}, {0,1}, {0,2}, {1,2} }, // L
    { {1,0}, {1,1}, {1,2}, {0,2} }, // L-rev
    { {0,0}, {1,0}, {2,0}, {1,1} }  // T
};

bool gridBlocks[GRID_COLUMNS][GRID_ROWS];

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

    void move(int, int);
    void draw(SDL_Renderer*);
    bool collisionCheck();
    void rotate();
};

void Tetromino::draw(SDL_Renderer* renderer) {
    for (auto block: shape) {
        SDL_Rect square = { x * CELL_SIZE + block[0] * CELL_SIZE, y * CELL_SIZE + block[1] * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &square);
    }
}

void spawnTetromino(Tetromino* tetromino) {

    for (auto block: tetromino->shape) {
        gridBlocks[tetromino->x + block[0]][tetromino->y + block [1]] = true;
    }

    tetromino->shape = SHAPES[3];
    tetromino->x = GRID_COLUMNS / 2;
    tetromino->y = 0;

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

void drawGrid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for (int i = 0; i <= GRID_COLUMNS; ++i) {
        int x = i * CELL_SIZE;
        SDL_RenderDrawLine(renderer, x, 0, x, GRID_ROWS * CELL_SIZE);
    }
    
    for (int i = 0; i <= GRID_ROWS; ++i) {
        int y = i * CELL_SIZE;
        SDL_RenderDrawLine(renderer, 0, y, GRID_COLUMNS * CELL_SIZE, y);
    }
}

void drawBlocks(SDL_Renderer* renderer) {
    for (int i = 0; i < GRID_COLUMNS; ++i) {
        for (int j = 0; j < GRID_ROWS; ++j) {
            if (gridBlocks[i][j]){
                SDL_Rect square = { i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                SDL_RenderFillRect(renderer, &square);
            }
        }
    }
}

void clearRow(int row) {
    for (int i = 0; i < GRID_COLUMNS; ++i) {
        for (int j = row; j > 0; --j) {
            gridBlocks[i][j] = gridBlocks[i][j-1];
        }
    }

    for (int i = 0; i < GRID_COLUMNS; ++i) {
        gridBlocks[i][0] = false;
    }
}

int checkForCompletion() {
    int n, nCompleted = 0;

    for (int i = 0; i < GRID_ROWS; ++i) {
        n = 0;

        for (int j = 0; j < GRID_COLUMNS; ++j) {
            if (gridBlocks[j][i]) ++n;
        }

        if (n == GRID_COLUMNS) { 
            ++nCompleted;
            clearRow(i);
            --i;
        }
        
    }

    return nCompleted;
}

void refresh(Engine *engine, Tetromino tetromino) {
    SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 255);
    SDL_RenderClear(engine->renderer);

    drawGrid(engine->renderer);

    drawBlocks(engine->renderer);

    tetromino.draw(engine->renderer);

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

    Tetromino tetromino = Tetromino(SHAPES[2]);

    std::memset(gridBlocks, 0, sizeof(gridBlocks));

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
                    tetromino.move(-1,0);
                    refresh(engine, tetromino);
                    break;
                case SDLK_s:
                    tetromino.move(0,1);
                    refresh(engine, tetromino);
                    break;
                case SDLK_d:
                    tetromino.move(1,0);
                    refresh(engine, tetromino);
                    break;
                case SDLK_w:
                    tetromino.rotate();
                    refresh(engine, tetromino);
                    break;
                }
            }

        }

        std::cout<<"["<<checkForCompletion()<<"[";

        refresh(engine, tetromino);

        tetromino.move(0,1);

        SDL_Delay(GAME_SPEED);

    }

    engine->~Engine();

    return 0;
}
