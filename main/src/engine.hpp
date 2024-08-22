#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL.h>

class Engine {

public:
    SDL_Window* window;
    SDL_Renderer* renderer;

    Engine();
    ~Engine();

    bool init(int screen_width, int screen_eight);


};

#endif