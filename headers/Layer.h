#ifndef LAYER_H
#define LAYER_H

#include "ILayer.h"
#include "Handlers.h"

template<WidgetType MainWidget
        , template<typename, typename...> class Interactor
        , CreateRequestType CreateRequest
        , ResponseHandler... Handlers>
class Layer final : public ILayer {
public:
    Layer (CreateRequest&& req, Handlers&&... handlers)
    : m_widget(std::move(req.widget))
    , m_interactor{std::move(req.payload), std::ref(m_widget), std::ref(m_pendingRequest)}
    , m_handlers{handlers...}
    , m_dispatcher{} 
    {}

    /// Layer's interface ///
    void dispatchEvents(const GuiEvent& event) override {
        std::visit([&](auto&& ev) { m_interactor.processEvents(ev); }, event);
    }

    void draw(SDL_Renderer* const renderer, const TextRenderer& textRenderer) const override {
        m_widget.render(renderer, textRenderer);
        m_interactor.render(renderer, textRenderer);
    }

    // dispatch responses to a correct action handler
    void onResponse(Responses&& resp) override {
        if constexpr (std::is_same_v<CreateRequest, NonModalLayerCreateRequest>) {
            std::apply([&](auto&... handlers) {
                m_dispatcher.dispatch(
                    std::move(resp)
                    , HandlerContext{ std::ref(m_widget), std::ref(m_pendingRequest), m_interactor.getOperationView()}
                    , handlers...
                    );
                }, m_handlers);
            m_interactor.processOperation();
        }
    }

    constexpr bool hitTest(float x, float y) const noexcept override {
        if constexpr (std::is_same_v<CreateRequest, NonModalLayerCreateRequest>) {
            return m_widget.contains(x, y);
        } else return true;
    }
protected:
    // main widget of the layer
    MainWidget m_widget;
    // main interactor of the layer with the widget
    Interactor<MainWidget, Handlers...> m_interactor;

    // action handlers of the layer and a dispatcher for handlers
    // If a layer is modal, then they should not exist
    [[no_unique_address]] std::tuple<Handlers...> m_handlers;
    [[no_unique_address]] ResponseDispatcher<Handlers...> m_dispatcher;
};

#endif
