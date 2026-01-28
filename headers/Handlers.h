#ifndef HANDLERS_H
#define HANDLERS_H
#include "Requests.h"
#include "Responses.h"

// bare minimum context that is necessary for 
// an action handlet
struct HandlerContext {
    std::reference_wrapper<OptRequest> req;
    std::reference_wrapper<const Widget> widget;
};

template<typename H, typename R>
concept CanHandleResponse =
    requires(H& h, R&& r, HandlerContext ctx) {
        { h.dispatch(static_cast<R&&>(r), ctx)} -> std::same_as<bool>;
        { H::getName() } -> std::convertible_to<std::string>;
    };

template<typename H, typename Variant>
struct HandlesAny;

template<typename H, typename... Rs>
struct HandlesAny<H, std::variant<Rs...>>
    : std::bool_constant<(CanHandleResponse<H, Rs> || ...)> {};

template<typename H>
concept ResponseHandler =
    HandlesAny<H, Responses>::value;

// This class is responsible for dispatching responses to a correct
// action handler
template<ResponseHandler... Handlers>
struct ResponseDispatcher {
    static bool dispatch(Responses&& r, HandlerContext ctx, Handlers&... hs) {
        return std::visit([&](auto&& resp) -> bool {
            using R = std::remove_cvref_t<decltype(resp)>;

            return (( [&]<typename H>(H& h) {
                if constexpr (CanHandleResponse<H, R>)
                    return h.dispatch(static_cast<decltype(resp)&&>(resp),ctx);
                else return false;
                }(hs) ) || ...);
        }, std::forward<Responses>(r));
    }
};

using EmptyResponseDispatcher = ResponseDispatcher<>;

// Handles actions of the "File" menu
class FileActionHandler {
    // actions of the "File" menu 
    enum class Actions : ActionID { OPEN = 0x01, SAVE = 0x02, CLOSE = 0x03 };
public:
    static constexpr std::string getName() { return "File"; }

    // called when File menu is first invoked to display possible actions
    bool requestMainMenu(HandlerContext);
    // receive a chosen option from the menu
    bool dispatch(MenuResponse&&, HandlerContext);
    // receive the file path from the dialog
    bool dispatch(DialogResponse&&, HandlerContext);
private:
    Actions m_currentAction;
};

// Handles actions of the "Help" menu
class HelpActionHandler {
    // acrions of the Help menu
    enum class Actions : ActionID { VIEW_GUIDE = 0x10};
public:
    static constexpr std::string getName() { return "Help"; }

    // called when Help menu is first invoked to display possible actions
    bool requestMainMenu(HandlerContext);
    // receives the chosen action.
    bool dispatch(MenuResponse&&, HandlerContext);
};

#endif 
