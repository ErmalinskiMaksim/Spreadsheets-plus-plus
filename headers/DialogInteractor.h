#ifndef DIALOG_INTERACTOR_H
#define DIALOG_INTERACTOR_H

#include "Events.h"
#include "Requests.h"
#include "Widget.h"

template<WidgetType MainWidget>
class DialogInteractor {
    using WidgetView = std::reference_wrapper<MainWidget>;
public:
    DialogInteractor(DialogCreateRequest::Payload&& payload, WidgetView widget, RequestView req) 
    : m_title{std::move(payload.title)}
    , m_input{payload.initInput ? std::move(payload.initInput) : ""}
    , r_widget{widget}
    , r_pendingRequest{req}
    , m_maxInputLen{payload.maxInputLen}
    {}

    template<GuiEventType Event>
    void processEvents(const Event&) {}

    void processEvents(const KeyUpEvent& event) {
        // enter = finish the dialog 
        // backspace = delete last character
        if(event.key == SDLK_RETURN) {
            r_pendingRequest.get() = !m_input->empty() 
                ? DialogCloseRequest{ DialogResponse{ std::move(*m_input) }}
                : DialogCloseRequest{ std::nullopt };
        } else if (event.key == SDLK_BACKSPACE) {
            if (!m_input->empty()) m_input->pop_back();
        }    
    }

    void processEvents(const TextInputEvent& event) {
        // append input text
        if (m_input && m_input->length() < m_maxInputLen)
            m_input->append(event.text);
    }

    void render(SDL_Renderer* renderer, const TextRenderer& textRenderer) const {
        auto hbox = r_widget.get().getHitBox();

        //  if there is no title draw input text at the top, else below the title
        auto dy = (m_title) ? textRenderer.getCharacterHeight() : 0;
        if (m_title) textRenderer.render(renderer, hbox, m_title->c_str(), m_title->length());
        textRenderer.render(renderer, {hbox.x, hbox.y+dy, hbox.w, hbox.h}, m_input->c_str(),  m_input->length());
    }
private:
    // title of the dialog (may or may not be)
    std::optional<std::string> m_title;
    // input text (can either be filled at first or empty)
    std::optional<std::string> m_input;

    WidgetView r_widget;
    RequestView r_pendingRequest;

    // maximum input lenght
    uint8_t m_maxInputLen;
};

#endif
