#include "Events.h"

OptGuiEvent translateEvent(const SDL_Event& event) noexcept {
    if (event.type == SDL_EVENT_MOUSE_MOTION) return MouseMotionEvent{event.motion.x, event.motion.y};
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) return MouseLeftDownEvent{event.button.x, event.button.y};
        if (event.button.button == SDL_BUTTON_RIGHT) return MouseRightDownEvent{event.button.x, event.button.y};
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event.button.button == SDL_BUTTON_LEFT) return MouseLeftUpEvent{event.button.x, event.button.y};
        if (event.button.button == SDL_BUTTON_RIGHT) return MouseRightUpEvent{event.button.x, event.button.y};
    }
    if (event.type == SDL_EVENT_KEY_UP) {
        if (event.key.key == SDLK_Q && event.key.mod & SDL_KMOD_CTRL)
            return QuitEvent{};
        return KeyUpEvent{event.key.key, event.key.mod};
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        return (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
            ? MouseScrollingEvent{-event.wheel.x, -event.wheel.y}
            : MouseScrollingEvent{event.wheel.x, event.wheel.y};
    }
    if (event.type == SDL_EVENT_TEXT_INPUT) return TextInputEvent{event.text.text};
    if (event.type == SDL_EVENT_QUIT) return QuitEvent{};
    return std::nullopt;
}
