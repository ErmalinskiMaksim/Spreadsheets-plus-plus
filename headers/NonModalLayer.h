#ifndef NON_MODAL_LAYER_H
#define NON_MODAL_LAYER_H

#include "Layer.h"
#include "Handlers.h"

template<WidgetType MainWidget, template<typename, typename...> class Interactor, ResponseHandler... Handlers>
class NonModalLayer final : public Layer {
public:
    NonModalLayer (Widget&& widget, Handlers&&... handlers)
    : m_widget(std::move(widget))
    , m_interactor{std::ref(m_widget), std::ref(m_pendingRequest)}
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
        std::apply([&](auto&... handlers) {
            m_dispatcher.dispatch(
                std::move(resp)
                , HandlerContext{ std::ref(m_widget), std::ref(m_pendingRequest), m_interactor.getOperationView()}
                , handlers...
            );
        }, m_handlers);
        m_interactor.processOperation();
    }

    bool hitTest(float x, float y) const noexcept override {
        return m_widget.contains(x, y);
    }
protected:
    // main widget of the layer
    MainWidget m_widget;
    // main interactor of the layer with the widget
    Interactor<MainWidget, Handlers...> m_interactor;

    // action handlers of the layer and a dispatcher for handlers
    std::tuple<Handlers...> m_handlers;
    ResponseDispatcher<Handlers...> m_dispatcher;
};

#endif
