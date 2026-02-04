#ifndef EVENTS_H
#define EVENTS_H

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <optional>
#include <variant>

// Internal events 

struct KeyUpEvent {
    Uint32 key;
    Uint16 mod;
};
struct MouseLeftUpEvent {
    float x; float y;
};
struct MouseLeftDownEvent {
    float x; float y;
};
struct MouseRightUpEvent {
    float x; float y;
};
struct MouseMotionEvent {
    float x; float y;
};
struct MouseScrollingEvent {
    float x; float y;
};
struct TextInputEvent {
    const char* text;
};
struct QuitEvent {};

template<typename T>
concept LayerEventType = std::is_same_v<T, KeyUpEvent>
                    || std::is_same_v<T, MouseLeftUpEvent>
                    || std::is_same_v<T, MouseLeftDownEvent>
                    || std::is_same_v<T, MouseRightUpEvent>
                    || std::is_same_v<T, MouseMotionEvent>
                    || std::is_same_v<T, MouseScrollingEvent>
                    || std::is_same_v<T, TextInputEvent>;

using LayerEvent = std::variant<
                  KeyUpEvent
                , MouseLeftUpEvent
                , MouseLeftDownEvent
                , MouseRightUpEvent
                , MouseMotionEvent
                , MouseScrollingEvent 
                , TextInputEvent
>;
 
using GuiEvent = std::variant<
                  KeyUpEvent
                , MouseLeftUpEvent
                , MouseLeftDownEvent
                , MouseRightUpEvent
                , MouseMotionEvent
                , MouseScrollingEvent
                , TextInputEvent
                , QuitEvent
>;

using OptGuiEvent = std::optional<GuiEvent>;

// convert SDL events to internal events
inline OptGuiEvent translateEvent(const SDL_Event& event) noexcept {
    switch (event.type) {
        case SDL_EVENT_MOUSE_MOTION:
            return MouseMotionEvent{event.motion.x, event.motion.y};
        case SDL_EVENT_MOUSE_WHEEL:
            // return (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
            return MouseScrollingEvent{event.wheel.x, event.wheel.y}; 
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            switch (event.button.button) {
                case SDL_BUTTON_LEFT: return MouseLeftDownEvent{event.button.x, event.button.y};
                default: return std::nullopt;
            }
        case SDL_EVENT_MOUSE_BUTTON_UP:
            switch (event.button.button) {
                case SDL_BUTTON_LEFT: return MouseLeftUpEvent{event.button.x, event.button.y};
                case SDL_BUTTON_RIGHT: return MouseRightUpEvent{event.button.x, event.button.y};
                default: return std::nullopt;
            }
        case SDL_EVENT_TEXT_INPUT:
            return TextInputEvent{event.text.text};
        case SDL_EVENT_KEY_UP:
            if (event.key.key == SDLK_Q && event.key.mod & SDL_KMOD_CTRL)
                return QuitEvent{};
            return KeyUpEvent{event.key.key, event.key.mod};
        case SDL_EVENT_QUIT:
            return QuitEvent{};
        default: return std::nullopt;
    }
}

#endif // !EVENTS_H
