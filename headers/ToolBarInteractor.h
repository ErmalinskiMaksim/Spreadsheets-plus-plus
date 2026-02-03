#ifndef TOOL_BAR_INTERACTOR_H
#define TOOL_BAR_INTERACTOR_H

#include "Events.h"
#include "Handlers.h"
#include <tuple>
#include <array>

struct Icon{};

template<WidgetType MainWidget, ResponseHandler... Handlers>
class ToolBarInteractor {
using WidgetView = std::reference_wrapper<MainWidget>;

    template <std::size_t I>
    using Handler = std::tuple_element_t<I, std::tuple<Handlers...>>;
public:
    ToolBarInteractor(NonModalLayerCreateRequest::Payload&&, WidgetView widget, RequestView req) 
    : m_tools{Handlers::getID()...}
    , m_buttons{}
    , m_operation{EmptyOperation{}}
    , r_widget{widget}
    , r_pendingRequest{req}
    {}
    template<GuiEventType Event>
    void processEvents(const Event&) {}

    void processEvents(const MouseLeftDownEvent& event) {
        processImpl(event, std::make_index_sequence<sizeof...(Handlers)>{});
    }

    OperationView getOperationView() {
        return std::ref(m_operation);
    }

    void processOperation() {}

    void render(SDL_Renderer* const, const TextRenderer&) const {
        // render button icons 
    }
private:
    template<size_t... Is>
    void processImpl(const MouseLeftDownEvent& event, std::index_sequence<Is...>) {
        ((Widget::contains(event.x, event.y, m_buttons[Is]) &&
          Handler<Is>::requestMainMenu(
              HandlerContext{r_widget.get(), r_pendingRequest, getOperationView() })
         ), ...);
    }

    // text names of menu buttons
    std::array<Icon, sizeof...(Handlers)> m_tools;
    // menu buttons' hit boxes
    std::array<SDL_FRect, sizeof...(Handlers)> m_buttons;

    OperationRegister m_operation;
    WidgetView r_widget;
    RequestView r_pendingRequest;
};

#endif
