#ifndef TETRIS_CONFIG
#define TETRIS_CONFIG

#include <vector>
#include <array>
#include <SDL.h>

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

#endif