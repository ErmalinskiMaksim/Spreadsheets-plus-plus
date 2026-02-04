#ifndef POPUP_INTERACTOR_H
#define POPUP_INTERACTOR_H

#include "Interactor.h"
#include "Requests.h"
#include "Widget.h"

template<WidgetType MainWidget>
class PopupInteractor : public Interactor {
    using WidgetView = std::reference_wrapper<MainWidget>;
    using TextLines = decltype(PopupCreateRequest::Payload::lines);
public:
    PopupInteractor(PopupCreateRequest::Payload&& payload, WidgetView widget, RequestView req) 
    : m_text{std::move(payload.lines)}
    , r_widget{widget}
    , r_pendingRequest{req}
    {}

    void dispatchEvents(const LayerEvent& event) {
        std::visit([&](auto&& ev) { processEvents(ev); }, event);
    }
    
    void render(SDL_Renderer* renderer, const TextRenderer& textRenderer) const {
        auto hbox = r_widget.get().getHitBox();
        
        // draw text content of the pop up
        auto dy = textRenderer.getCharacterHeight();
        for (auto i = 0uz; i < m_text.size(); ++i) 
            textRenderer.render(renderer, { hbox.x, hbox.y + static_cast<float>(i)*dy, hbox.w, dy}
                    , m_text[i].data(), m_text[i].length());
    }
private:
    using Interactor::processEvents;
    void processEvents(const MouseLeftDownEvent& event) {
        // on LMB click request todestroy itself
        if (r_widget.get().contains(event.x, event.y))
            r_pendingRequest.get() = PopupCloseRequest{};
    }
    
    TextLines m_text;

    WidgetView r_widget;
    RequestView r_pendingRequest;
};

#endif
