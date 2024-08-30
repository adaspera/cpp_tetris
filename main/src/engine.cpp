#include <engine.hpp>
#include <iostream>



Engine::Engine() {
    window = nullptr;
    renderer = nullptr;
    font = nullptr;
}

Engine::~Engine() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Engine::init(int screen_width, int screen_height) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initialize. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    return true;
}

bool Engine::initFont() {

    if ( TTF_Init() < 0 ) {
	    std::cerr << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }
    
    font = TTF_OpenFont("main/font/impact.ttf", 24);

    if ( !font ) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}