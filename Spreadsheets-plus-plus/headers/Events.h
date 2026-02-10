#ifndef EVENTS_H
#define EVENTS_H

#include "DepsEvents.h"
#include <optional>
#include <variant>

// Internal events 

struct KeyUpEvent {
    KeyType key;
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
    TextType text;
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

// convert events to internal events
inline OptGuiEvent translateEvent(const LibEvent& event) noexcept {
    switch (EVENT_TYPE(event)) {
        case EVENT_MOUSE_MOTION:
            return MouseMotionEvent{MOUSE_MOTION_X(event), MOUSE_MOTION_Y(event)};
        case EVENT_MOUSE_WHEEL:
            return MouseScrollingEvent{MOUSE_WHEEL_X(event), MOUSE_WHEEL_Y(event)}; 
        case EVENT_MOUSE_BUTTON_DOWN:
            switch (MOUSE_BUTTON_TYPE(event)) {
                case MOUSE_BUTTON_LEFT: return MouseLeftDownEvent{MOUSE_BUTTON_X(event), MOUSE_BUTTON_Y(event)};
                default: return std::nullopt;
            }
        case EVENT_MOUSE_BUTTON_UP:
            switch (MOUSE_BUTTON_TYPE(event)) {
                case MOUSE_BUTTON_LEFT: return MouseLeftUpEvent{MOUSE_BUTTON_X(event), MOUSE_BUTTON_Y(event)};
                case MOUSE_BUTTON_RIGHT: return MouseRightUpEvent{MOUSE_BUTTON_X(event), MOUSE_BUTTON_Y(event)};
                default: return std::nullopt;
            }
        case EVENT_TEXT_INPUT:
            return TextInputEvent{TEXT_INPUT_TEXT(event)};
        case EVENT_KEY_UP:
            if (KEY_KEY(event) == KEY_Q && IS_KEYMOD_CTRL(event))
                return QuitEvent{};
            return KeyUpEvent{KEY_KEY(event)};
        case EVENT_QUIT:
            return QuitEvent{};
        default: return std::nullopt;
    }
}

#endif // !EVENTS_H
