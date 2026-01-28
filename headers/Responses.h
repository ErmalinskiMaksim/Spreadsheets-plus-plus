#ifndef RESPONSES_H
#define RESPONSES_H

#include <string>
#include <variant>
#include <cstdint>

// Responses are a means of communication between layers.
// They are used in the request/response mechanism. For 
// instance, when a layer wants to request GUI to close it,
// it may need to communicate some data to the layer that
// requested its creation. 
// E.g. a menu after being clicked needs to tell the layer 
// that requested the menu which action was chosen, etc.

using ActionID = uint8_t;

struct MenuResponse {
    ActionID code;
};

struct DialogResponse {
    std::string text;
};

struct PopupResponse {};
template<typename T>
concept ResponseType = std::is_same_v<T, MenuResponse>
                    || std::is_same_v<T, DialogResponse>
                    || std::is_same_v<T, PopupResponse>;
using Responses = std::variant<MenuResponse
                    , DialogResponse
                    , PopupResponse>;

#endif
