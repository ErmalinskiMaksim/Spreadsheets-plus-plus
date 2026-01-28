#ifndef BAR_LAYER_H
#define BAR_LAYER_H
#include "Layer.h"
#include "Handlers.h"
#include "TextRenderer.h"
#include <array>
#include <tuple>

// A 100% generic layer assosiated to task bars.
// To add a new button to it, it's necessary to 
// create a handler for it and pass it as a template
// parameter
template<ResponseHandler... Handlers>
class TaskBarLayer final : public Layer {
public:
    TaskBarLayer(Widget&& widget, Handlers&&... handlers)
    : m_tasks{Handlers::getName()...}
    , m_buttons{}
    , m_widget(std::move(widget))
    , m_handlers(handlers...)
    , m_dispatcher{} 
    {
        auto hbox = m_widget.getHitBox();
        auto w = m_widget.getCharWidth();
        for (auto i = 0uz; i < m_buttons.size(); ++i) {
            m_buttons[i] = {hbox.x, hbox.y, w * static_cast<float>(m_tasks[i].size()), hbox.h };
            hbox.x += m_buttons[i].w;
        }
    }
    /// Layer's interface ///
    void dispatchEvents(const GuiEvent& event) override {
        std::visit([&](auto&& ev) { processEvents(ev); }, event);
    }

    void draw(SDL_Renderer* const renderer, const TextRenderer& textRenderer) const override {
        auto hbox = m_widget.getHitBox();
        auto c = m_widget.getColor();

        // draw background
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &hbox);

        // render button text
        for (auto i = 0uz; i < m_tasks.size(); ++i) 
            textRenderer.render(renderer, m_buttons[i], m_tasks[i].c_str(), m_tasks[i].length());
    }

    // dispatch responses to a correct action handler
    void onResponse(Responses&& resp) override {
        std::apply([&](auto&... handlers) {
            m_dispatcher.dispatch(
                std::move(resp)
                , HandlerContext{ std::ref(m_pendingRequest),std::ref(m_widget)}
                , handlers...
            );
        }, m_handlers);
    }

    bool hitTest(float x, float y) const noexcept override {
        return m_widget.contains(x, y);
    }
protected:
    // supported evetns
    using Layer::processEvents;
    void processEvents(const MouseLeftDownEvent& event) {
        processImpl(event, std::make_index_sequence<sizeof...(Handlers)>{});
    }

    // generically process events of all buttons
    template<size_t... Is>
    void processImpl(const MouseLeftDownEvent& event, std::index_sequence<Is...>) {
        auto widget = m_widget;

        ((Widget::contains(event.x, event.y, m_buttons[Is]) &&
          std::get<Is>(m_handlers).requestMainMenu(
              HandlerContext{std::ref(m_pendingRequest), std::ref(widget)})
         ), ...);
    }
    
    // text names of menu buttons
    std::array<std::string, sizeof...(Handlers)> m_tasks;
    // menu buttons' hit boxes
    std::array<SDL_FRect, sizeof...(Handlers)> m_buttons;
    // basic widget data
    Widget m_widget;

    // action handlers of each button
    std::tuple<Handlers...> m_handlers;
    ResponseDispatcher<Handlers...> m_dispatcher;
};

#endif
