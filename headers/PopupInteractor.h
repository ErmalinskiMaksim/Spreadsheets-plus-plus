#ifndef POPUP_INTERACTOR_H
#define POPUP_INTERACTOR_H

#include "Events.h"
#include "Requests.h"
#include "Widget.h"

template<WidgetType MainWidget>
class PopupInteractor {
    using WidgetView = std::reference_wrapper<MainWidget>;
    using TextLines = std::vector<std::string>;
public:
    PopupInteractor(PopupCreateRequest::Payload&& payload, WidgetView widget, RequestView req) 
    : m_text{std::move(payload.lines)}
    , r_widget{widget}
    , r_pendingRequest{req}
    {}

    template<GuiEventType Event>
    void processEvents(const Event&) {}

    void processEvents(const MouseLeftDownEvent& event) {
        // on LMB click request todestroy itself
        if (r_widget.get().contains(event.x, event.y))
            r_pendingRequest.get() = PopupCloseRequest{};
    }

    void render(SDL_Renderer* renderer, const TextRenderer& textRenderer) const {
        auto hbox = r_widget.get().getHitBox();
        
        // draw text content of the pop up
        auto dy = textRenderer.getCharacterHeight();
        for (auto i = 0uz; i < m_text.size(); ++i) 
            textRenderer.render(renderer, { hbox.x, hbox.y + static_cast<float>(i)*dy, hbox.w, dy}
                    , m_text[i].c_str(), m_text[i].length());
    }
private:
    TextLines m_text;

    WidgetView r_widget;
    RequestView r_pendingRequest;
};

#endif
