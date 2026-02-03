#ifndef REQUESTS_H
#define REQUESTS_H

#include <functional>
#include <type_traits>
#include <vector>
#include <optional>
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

struct NonModalLayerCreateRequest {
    Widget widget;
    struct Payload{} payload;
};

struct MenuCreateRequest {
    struct Payload {
        struct MenuAction {
            std::string text;
            ActionID id;
        };
        std::vector<MenuAction> entries;
    };
    Widget widget;
    Payload payload;
};

struct MenuCloseRequest {
    std::optional<MenuResponse> resp;    
};

struct DialogCreateRequest {
    struct Payload {
        std::optional<std::string> title;
        std::optional<std::string> initInput;
        uint8_t maxInputLen;
    };
    Widget widget;
    Payload payload;
};

struct DialogCloseRequest {
    std::optional<DialogResponse> resp;
};

struct PopupCreateRequest {
    struct Payload {
        std::vector<std::string> lines;
    };
    Widget widget;
    Payload payload; 
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
using RequestView = std::reference_wrapper<OptRequest>;

template<typename T>
concept RequestType = std::is_same_v<T, NonModalLayerCreateRequest>
                    || std::is_same_v<T, MenuCreateRequest> 
                    || std::is_same_v<T, MenuCloseRequest>
                    || std::is_same_v<T, DialogCreateRequest>
                    || std::is_same_v<T, DialogCloseRequest>
                    || std::is_same_v<T, PopupCreateRequest>
                    || std::is_same_v<T, PopupCloseRequest>
                    ;

template<typename T>
concept CreateRequestType = std::is_same_v<T, NonModalLayerCreateRequest>
                        || std::is_same_v<T, MenuCreateRequest>
                        || std::is_same_v<T, DialogCreateRequest>
                        || std::is_same_v<T, PopupCreateRequest>;

#endif
