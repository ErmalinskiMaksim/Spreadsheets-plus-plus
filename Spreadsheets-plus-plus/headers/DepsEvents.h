#ifndef DEPS_EVENTS_H
#define DEPS_EVENTS_H

#ifdef USE_SDL

#include "SDL3/SDL_events.h"

using LibEvent = SDL_Event;
inline LibEvent waitEvent() {
    LibEvent ev;
    SDL_WaitEvent(&ev);
    return ev;
}
 
inline auto GetMouseState(float* x, float* y) {
    return SDL_GetMouseState(x, y);
}

// EVENT TYPES
#define EVENT_TYPE(ev)          (ev.type)
#define EVENT_QUIT              SDL_EVENT_QUIT

// MOUSE BUTTON
#define EVENT_MOUSE_BUTTON_DOWN SDL_EVENT_MOUSE_BUTTON_DOWN
#define EVENT_MOUSE_BUTTON_UP   SDL_EVENT_MOUSE_BUTTON_UP
#define MOUSE_BUTTON_LEFT       SDL_BUTTON_LEFT
#define MOUSE_BUTTON_RIGHT      SDL_BUTTON_RIGHT
#define MOUSE_BUTTON_TYPE(ev)   (ev.button.button)
#define MOUSE_BUTTON_X(ev)      (ev.button.x)
#define MOUSE_BUTTON_Y(ev)      (ev.button.y)

// MOUSE MOTION
#define EVENT_MOUSE_MOTION      SDL_EVENT_MOUSE_MOTION
#define MOUSE_MOTION_X(ev)      (ev.motion.x)
#define MOUSE_MOTION_Y(ev)      (ev.motion.y)

// MOUSE WHEEL
#define EVENT_MOUSE_WHEEL       SDL_EVENT_MOUSE_WHEEL
#define MOUSE_WHEEL_X(ev)       (ev.wheel.x)
#define MOUSE_WHEEL_Y(ev)       (ev.wheel.y)

// KEYBOARD
#define EVENT_KEY_UP            SDL_EVENT_KEY_UP
#define KEY_Q                   SDLK_Q
#define KEY_ENTER               SDLK_RETURN
#define KEY_BACKSPACE           SDLK_BACKSPACE
#define KEYMOD_CTRL             SDL_KMOD_CTRL
#define KEY_KEY(ev)             (ev.key.key)
#define IS_KEYMOD_CTRL(ev)      (ev.key.mod & KEYMOD_CTRL)
using KeyType = Uint32;
using ModType = Uint16;

inline bool isCTRL() {
    return SDL_GetModState() & KEYMOD_CTRL;
}

// TEXT INPUT
#define EVENT_TEXT_INPUT        SDL_EVENT_TEXT_INPUT
#define TEXT_INPUT_TEXT(ev)     (ev.text.text)
using TextType = const char*;

#elif USE_SFML

#include "SFML/Window/Event.hpp"

using LibEvent = sf::Event;
inline auto GetMouseState(float* x, float* y) {
    auto pos = sf::Mouse::getPosition();
    *x = static_cast<float>(pos.x);
    *y = static_cast<float>(pos.y);
}

inline bool isCTRL() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || 
    sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
}

// EVENT TYPES
#define EVENT_TYPE(ev)          (ev.type)
#define EVENT_QUIT              sf::Event::Closed

// MOUSE BUTTON
#define EVENT_MOUSE_BUTTON_DOWN sf::Event::MouseButtonPressed
#define EVENT_MOUSE_BUTTON_UP   sf::Event::MouseButtonReleased
#define MOUSE_BUTTON_LEFT       sf::Mouse::Left
#define MOUSE_BUTTON_RIGHT      sf::Mouse::Right
#define MOUSE_BUTTON_TYPE(ev)   (ev.mouseButton.button)
#define MOUSE_BUTTON_X(ev)      (static_cast<float>(ev.mouseButton.x))
#define MOUSE_BUTTON_Y(ev)      (static_cast<float>(ev.mouseButton.y))

// MOUSE MOTION
#define EVENT_MOUSE_MOTION      sf::Event::MouseMoved
#define MOUSE_MOTION_X(ev)      (static_cast<float>(ev.mouseMove.x))
#define MOUSE_MOTION_Y(ev)      (static_cast<float>(ev.mouseMove.y))

// MOUSE WHEEL
#define EVENT_MOUSE_WHEEL       sf::Event::MouseWheelScrolled
#define MOUSE_WHEEL_X(ev)       (ev.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel ? static_cast<float>(ev.mouseWheelScroll.delta) : 0.0f)
#define MOUSE_WHEEL_Y(ev)       (ev.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel ? static_cast<float>(ev.mouseWheelScroll.delta) : 0.0f)

// KEYBOARD
#define EVENT_KEY_UP            sf::Event::KeyReleased
#define KEY_Q                   sf::Keyboard::Q
#define KEY_ENTER               sf::Keyboard::Enter
#define KEY_BACKSPACE           sf::Keyboard::BackSpace
#define KEY_KEY(ev)             (ev.key.code)
#define IS_KEYMOD_CTRL(ev)      (ev.key.control)
using KeyType = sf::Keyboard::Key;

// TEXT INPUT
#define EVENT_TEXT_INPUT        sf::Event::TextEntered
#define TEXT_INPUT_TEXT(ev)     (sf::String(event.text.unicode).toAnsiString())
using TextType = std::string;
#endif 

#endif
