#ifndef TOOL_BAR_INTERACTOR_H
#define TOOL_BAR_INTERACTOR_H

#include "Interactor.h"
#include "Handlers.h"
#include <tuple>
#include <array>

struct Icon{};

template<WidgetType MainWidget, ResponseHandler... Handlers>
class ToolBarInteractor : public Interactor {
    using WidgetView = std::reference_wrapper<MainWidget>;
    template <std::size_t I>
    using Handler = std::tuple_element_t<I, std::tuple<Handlers...>>;
public:
    static constexpr bool hasOperations = false;

    ToolBarInteractor(NonModalLayerCreateRequest::Payload&&, WidgetView widget, RequestView req) 
    : m_tools{Handlers::getID()...}
    , m_buttons{}
    , r_widget{widget}
    , r_pendingRequest{req}
    {}
    
    void dispatchEvents(const LayerEvent& event) {
        std::visit([&](auto&& ev) { processEvents(ev); }, event);
    }

    void render(SDL_Renderer* const, const TextRenderer&) const {
        // render button icons 
    }
private:
    using Interactor::processEvents;
    void processEvents(const MouseLeftDownEvent& event) {
        processImpl(event, std::make_index_sequence<sizeof...(Handlers)>{});
    
    }
    template<size_t... Is>
    void processImpl(const MouseLeftDownEvent& event, std::index_sequence<Is...>) {
        ((Widget::contains(event.x, event.y, m_buttons[Is]) &&
          Handler<Is>::requestMainMenu(
              HandlerContext{r_widget.get(), r_pendingRequest })
         ), ...);
    }

    // text names of menu buttons
    std::array<Icon, sizeof...(Handlers)> m_tools;
    // menu buttons' hit boxes
    std::array<SDL_FRect, sizeof...(Handlers)> m_buttons;

    WidgetView r_widget;
    RequestView r_pendingRequest;
};

#endif
