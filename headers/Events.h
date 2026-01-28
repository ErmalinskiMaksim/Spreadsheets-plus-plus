#ifndef EVENTS_H
#define EVENTS_H

#include <SDL3/SDL_events.h>
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
struct MouseRightDownEvent {
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
concept GuiEventType = std::is_same_v<T, KeyUpEvent>
                    || std::is_same_v<T, MouseLeftUpEvent>
                    || std::is_same_v<T, MouseLeftDownEvent>
                    || std::is_same_v<T, MouseRightUpEvent>
                    || std::is_same_v<T, MouseRightDownEvent>
                    || std::is_same_v<T, MouseMotionEvent>
                    || std::is_same_v<T, MouseScrollingEvent>
                    || std::is_same_v<T, TextInputEvent>
                    || std::is_same_v<T, QuitEvent>;

using GuiEvent = std::variant<
                  KeyUpEvent
                , MouseLeftUpEvent
                , MouseLeftDownEvent
                , MouseRightUpEvent
                , MouseRightDownEvent
                , MouseMotionEvent
                , MouseScrollingEvent 
                , TextInputEvent
                , QuitEvent
            >;

using OptGuiEvent = std::optional<GuiEvent>;

// convert SDL events to internal events
OptGuiEvent translateEvent(const SDL_Event& event) noexcept;

#endif // !EVENTS_H
