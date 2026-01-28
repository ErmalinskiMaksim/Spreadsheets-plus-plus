#ifndef REQUESTS_H
#define REQUESTS_H

#include <optional>
#include <vector>
#include "Responses.h"
#include "Widget.h"

// Requests are a mechanism that enables layers to communicate
// with GUI upstream. They are used to let any layer ask for a 
// new layer creation or deletion 
//
// They follow the same pattern: 
// * create requests contain everything necessary to construct
// a new layer
// * close requests contain a response from the layer that 
// requests to be destroyed

struct MenuCreateRequest {
    struct MenuAction {
        std::string text;
        ActionID id;
    };
    Widget widget;
    std::vector<MenuAction> entries;
};

struct MenuCloseRequest {
    std::optional<MenuResponse> resp;    
};

struct DialogCreateRequest {
    Widget widget;
    std::optional<std::string> title;
    std::optional<std::string> initInput;
    uint8_t maxInputLen;
};

struct DialogCloseRequest {
    std::optional<DialogResponse> resp;
};

struct PopupCreateRequest {
    Widget widget;
    std::vector<std::string> lines;
};

struct PopupCloseRequest {
    std::optional<PopupResponse> resp;
};

using Requests = std::variant<MenuCreateRequest
            , MenuCloseRequest
            , DialogCreateRequest
            , DialogCloseRequest
            , PopupCreateRequest
            , PopupCloseRequest
    >;  
using OptRequest = std::optional<Requests>;

template<typename T>
concept RequestType = std::is_same_v<T, MenuCreateRequest> 
                    || std::is_same_v<T, MenuCloseRequest>
                    || std::is_same_v<T, DialogCreateRequest>
                    || std::is_same_v<T, DialogCloseRequest>
                    || std::is_same_v<T, PopupCreateRequest>
                    || std::is_same_v<T, PopupCloseRequest>
                    ;

#endif
