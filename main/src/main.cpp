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
#include "Tetromino.cpp"

static Uint32 next_tick_time;
static Uint32 next_move_time;

Uint32 time_left(Uint32 next_time)
{
    Uint32 now;

    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
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
            if (gridBlocks[i][j].is){
                SDL_SetRenderDrawColor(renderer, 
                    gridBlocks[i][j].color.r, gridBlocks[i][j].color.g, gridBlocks[i][j].color.b, gridBlocks[i][j].color.a);
                    
                SDL_Rect square = { i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                SDL_RenderFillRect(renderer, &square);
            }
        }
    }
}

void clearRow(int row) {
    for (int i = 0; i < GRID_COLUMNS; ++i) {
        for (int j = row; j > 0; --j) {
            gridBlocks[i][j].is = gridBlocks[i][j-1].is;
        }
    }

    for (int i = 0; i < GRID_COLUMNS; ++i) {
        gridBlocks[i][0].is = false;
    }
}

int checkForCompletion() {
    int n, nCompleted = 0;

    for (int i = 0; i < GRID_ROWS; ++i) {
        n = 0;

        for (int j = 0; j < GRID_COLUMNS; ++j) {
            if (gridBlocks[j][i].is) ++n;
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

    if (DEBUG_CONSOLE) {
        AllocConsole();

        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        std::cout << "Debug console" << std::endl;
    }
    

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

        if (!time_left(next_move_time)) {
            tetromino.move(0,1);
            next_move_time += GAME_SPEED;
        }
            

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


        SDL_Delay(time_left(next_tick_time));
        next_tick_time += TICK_INTERVAL;

    }

    engine->~Engine();

    return 0;
}
