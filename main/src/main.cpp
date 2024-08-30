#include <SDL.h>
#include <iostream>
#include <vector>
#include <array>
#include <windows.h>
#include <cstring>
#include <ctime>
#include <SDL_ttf.h>
#include <string>
#include <cmath>
#include "engine.hpp"

const int GRID_COLUMNS = 10;
const int GRID_ROWS = 16;
const int CELL_SIZE = 30;
const int SCORE_WINDOW_WIDTH = 200;
const int GAME_SPEED = 200;

const int SCREEN_WIDTH = GRID_COLUMNS * CELL_SIZE + SCORE_WINDOW_WIDTH;
const int SCREEN_HEIGHT = GRID_ROWS * CELL_SIZE;

const std::vector<std::vector<std::array<int, 2>>> SHAPES = {
    { {0,0}, {0,1}, {0,2}, {0,3} }, // I
    { {0,0}, {0,1}, {1,0}, {1,1} }, // block
    { {0,0}, {1,0}, {1,1}, {2,1} }, // Z-1
    { {1,0}, {2,0}, {0,1}, {1,1} }, // Z-2
    { {0,0}, {0,1}, {0,2}, {1,2} }, // L
    { {1,0}, {1,1}, {1,2}, {0,2} }, // L-rev
    { {0,0}, {1,0}, {2,0}, {1,1} }  // T
};

const std::vector<SDL_Color> COLORS = {
     {240,0,0,255},   //Red
     {160,0,240,255}, //Purple
     {0,240,0,255},   //Green
     {240,240,0,255}, //Yellow
     {240,160,0,255}, //Orange
     {0,0,240,255},   //Blue
     {0,240,240,255}  //Cyan
};

bool gridBlocks[GRID_COLUMNS][GRID_ROWS];
int score;
bool gameOver;

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

void drawScoreboard(SDL_Renderer* renderer, TTF_Font* font) {
    
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* textTitle = TTF_RenderText_Solid( font, "CURRENT SCORE", color );

    std::string str = std::to_string(score);
    const char* cstr = str.c_str();
    SDL_Surface* textScore = TTF_RenderText_Solid( font, cstr, color );

    SDL_Texture* textTitleTexture = SDL_CreateTextureFromSurface( renderer, textTitle );
    SDL_Rect destTitle = { GRID_COLUMNS * CELL_SIZE + CELL_SIZE, CELL_SIZE, textTitle->w, textTitle->h };
    SDL_RenderCopy( renderer, textTitleTexture, NULL, &destTitle );

    SDL_Texture* textScoreTexture = SDL_CreateTextureFromSurface( renderer, textScore );
    SDL_Rect destScore = { GRID_COLUMNS * CELL_SIZE + CELL_SIZE, CELL_SIZE + textTitle->h + 10, textScore->w, textScore->h };
    SDL_RenderCopy( renderer, textScoreTexture, NULL, &destScore );
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

    drawScoreboard(engine->renderer, engine->font);

    SDL_RenderPresent(engine->renderer);
}

void DrawGameOver(Engine *engine) {
    SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 255);
    SDL_RenderClear(engine->renderer);

    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* textTitle = TTF_RenderText_Solid( engine->font, "Game Over", COLORS[0] );

    SDL_Surface* textTitle2 = TTF_RenderText_Solid( engine->font, "Final Score", color );

    std::string str = std::to_string(score);
    const char* cstr = str.c_str();
    SDL_Surface* textScore = TTF_RenderText_Solid( engine->font, cstr, color );

    SDL_Texture* textTitleTexture = SDL_CreateTextureFromSurface( engine->renderer, textTitle );
    SDL_Rect destTitle = { (GRID_COLUMNS * CELL_SIZE + SCORE_WINDOW_WIDTH) / 3, CELL_SIZE, textTitle->w, textTitle->h };
    SDL_RenderCopy( engine->renderer, textTitleTexture, NULL, &destTitle );

    SDL_Texture* textTitle2Texture = SDL_CreateTextureFromSurface( engine->renderer, textTitle2 );
    SDL_Rect destTitle2 = { (GRID_COLUMNS * CELL_SIZE + SCORE_WINDOW_WIDTH) / 3, CELL_SIZE + textTitle->h + 10, textTitle2->w, textTitle2->h };
    SDL_RenderCopy( engine->renderer, textTitle2Texture, NULL, &destTitle2 );

    SDL_Texture* textScoreTexture = SDL_CreateTextureFromSurface( engine->renderer, textScore );
    SDL_Rect destScore = { (GRID_COLUMNS * CELL_SIZE + SCORE_WINDOW_WIDTH) / 3 + 40, CELL_SIZE + textTitle->h + textTitle2->h + 10, textScore->w, textScore->h };
    SDL_RenderCopy( engine->renderer, textScoreTexture, NULL, &destScore );

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

    if (!engine->initFont()) {
        std::cerr << "Failed to initialize SDL_font." << std::endl;
        return -1;
    }

    Tetromino tetromino = Tetromino(SHAPES[2]);

    std::memset(gridBlocks, 0, sizeof(gridBlocks));
    score = 0;

    bool quit = false;
    gameOver = false;
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

        score += int(pow(checkForCompletion() * 10, 2));

        refresh(engine, tetromino);

        tetromino.move(0,1);

        std::cout<<"["<<gameOver<<"]";


        while (!quit && gameOver) {
            DrawGameOver(engine);
            SDL_Delay(100);

            while (SDL_PollEvent(&e) != 0) {
                switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    tetromino = Tetromino(SHAPES[2]);
                    std::memset(gridBlocks, 0, sizeof(gridBlocks));
                    score = 0;
                    gameOver = false;
                }

            }
        }


        SDL_Delay(GAME_SPEED);

    }

    engine->~Engine();

    return 0;
}
