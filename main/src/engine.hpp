#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL.h>
#include <SDL_ttf.h>

class Engine {

public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    Engine();
    ~Engine();

    bool init(int screen_width, int screen_eight);
    bool initFont();


};

#endif