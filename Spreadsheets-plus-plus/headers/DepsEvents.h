#ifndef DEPS_EVENTS_H
#define DEPS_EVENTS_H

#include "SDL3/SDL_events.h"

using LibEvent = SDL_Event;
#define EVENT_MOUSE_BUTTON_DOWN SDL_EVENT_MOUSE_BUTTON_DOWN
#define EVENT_MOUSE_BUTTON_UP   SDL_EVENT_MOUSE_BUTTON_UP
#define EVENT_MOUSE_MOTION      SDL_EVENT_MOUSE_MOTION
#define EVENT_MOUSE_WHEEL       SDL_EVENT_MOUSE_WHEEL
#define EVENT_TEXT_INPUT        SDL_EVENT_TEXT_INPUT
#define EVENT_KEY_UP            SDL_EVENT_KEY_UP
#define EVENT_QUIT              SDL_EVENT_QUIT

inline LibEvent waitEvent() {
    LibEvent ev;
    SDL_WaitEvent(&ev);
    return ev;
}

inline auto GetError() {
    return SDL_GetError();
} 

inline auto GetMouseState(float* x, float* y) {
    return SDL_GetMouseState(x, y);
}

inline auto GetModState() {
    return SDL_GetModState();
}
// MOUSE
#define MOUSE_BUTTON_LEFT       SDL_BUTTON_LEFT
#define MOUSE_BUTTON_RIGHT      SDL_BUTTON_RIGHT

// KEYBOARD
#define KEY_Q                   SDLK_Q
#define KEY_ENTER               SDLK_RETURN
#define KEY_BACKSPACE           SDLK_BACKSPACE
#define KEYMOD_CTRL             SDL_KMOD_CTRL
using KeyType = Uint32;
using ModType = Uint16;

#endif
