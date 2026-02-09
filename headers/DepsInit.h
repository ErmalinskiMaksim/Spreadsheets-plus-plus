#ifndef DEPS_INIT_H
#define DEPS_INIT_H

#include <SDL3_ttf/SDL_ttf.h>

class GUI;
class LibInitDeinit {
    friend GUI;
    static bool init() {
        constexpr SDL_InitFlags SDL_FLAGS = SDL_INIT_VIDEO;         
        return SDL_Init(SDL_FLAGS) && TTF_Init();
    }
    static void deinit() {
        TTF_Quit();
        SDL_Quit();
    }
};

#endif // !DEPS_INIT_H
