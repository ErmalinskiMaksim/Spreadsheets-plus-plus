#ifndef HANDLERS_H
#define HANDLERS_H

#include "Requests.h"

// global operation register. Those are the kinds of operations that 
// are performed by the caller upon handler completion. Hanlders were 
// initially supposed to handle static operations on global singletons 
// but then some specific kind of operations emerged which demanded a
// signal passing mechanism between callers and handlers.
// TODO maybe improve later
struct EmptyOperation {};
struct InsertOperation { bool isAfter; };
struct DeleteOperation {};
struct WriteOperation { std::string text; };
using OperationRegister = std::variant<EmptyOperation
            , InsertOperation
            , DeleteOperation
            , WriteOperation
>;
using OperationView = std::reference_wrapper<OperationRegister>;
     
// bare minimum context that is necessary for 
// an action handler
struct HandlerContext {
    Widget widget;
    RequestView req;
    OperationView op;
};

template<typename H, typename R>
concept CanHandleResponse =
    requires(H& h, R&& r, HandlerContext ctx) {
        { h.dispatch(static_cast<R&&>(r), ctx)} -> std::same_as<bool>;
        { H::getID() };
        { H::requestMainMenu(ctx) } -> std::same_as<bool>;
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

#endif 
