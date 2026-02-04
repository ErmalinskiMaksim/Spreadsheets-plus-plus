#ifndef MENU_INTERACTOR_H
#define MENU_INTERACTOR_H

#include "Interactor.h"
#include "Requests.h"
#include "Widget.h"

template<WidgetType MainWidget>
class MenuInteractor : public Interactor {
    using WidgetView = std::reference_wrapper<MainWidget>;
    using Entries = std::vector<MenuCreateRequest::Payload::MenuAction>;
public:
    MenuInteractor(MenuCreateRequest::Payload&& payload, WidgetView widget, RequestView req) 
    : m_entries{std::move(payload.entries)}
    , r_widget{widget}
    , r_pendingRequest{req}
    {}

    void dispatchEvents(const LayerEvent& event) {
        std::visit([&](auto&& ev) { processEvents(ev); }, event);
    }

    void render(SDL_Renderer* renderer, const TextRenderer& textRenderer) const {
        auto hbox = r_widget.get().getHitBox();

        // draw all the menu entries
        auto dy = textRenderer.getCharacterHeight();
        for (auto i = 0uz; i < m_entries.size(); ++i) 
            textRenderer.render(renderer, { hbox.x, hbox.y + static_cast<float>(i)*dy, hbox.w, dy}
                    , m_entries[i].text.data(), m_entries[i].text.length());
    }
private:
    using Interactor::processEvents;
    void processEvents(const MouseLeftDownEvent& event) {
        if (r_widget.get().contains(event.x, event.y)) {
            auto hbox = r_widget.get().getHitBox();
            auto idx = static_cast<size_t>((event.y - hbox.y) * static_cast<float>(m_entries.size()) / hbox.h);
            r_pendingRequest.get() = MenuCloseRequest{ MenuResponse{m_entries[idx].id} };
        } // if a click happened outside the widget assume that no action was chosen 
        // ans similarly send a close request
        else r_pendingRequest.get() = MenuCloseRequest{ std::nullopt };
    }
    
    Entries m_entries; 

    WidgetView r_widget;
    RequestView r_pendingRequest;
};

#endif
