#include "DialogLayer.h"
#include "Requests.h"
#include "Responses.h"
#include "TextRenderer.h"
#include <optional>

DialogLayer::DialogLayer(DialogCreateRequest&& req) 
    : m_title(std::move(req.title))
    , m_input{req.initInput ? std::move(req.initInput) : ""}
    , m_widget(std::move(req.widget))
    , m_maxInputLen(req.maxInputLen) 
{}

void DialogLayer::dispatchEvents(const GuiEvent& event) {
    std::visit([&](auto&& ev) { processEvents(ev); }, event);
}

void DialogLayer::draw(SDL_Renderer* const renderer, const TextRenderer& txtRenderer) const {
    auto hbox = m_widget.getHitBox();
    auto c = m_widget.getColor();

    // draw dialog background
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &hbox);

    // draw dialog outline
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderRect(renderer, &hbox);

    //  if there is no title draw input text at the top, else below the title
    auto dy = (m_title) ? txtRenderer.getCharacterHeight() : 0;
    if (m_title) txtRenderer.render(renderer, hbox, m_title->c_str(), m_title->length());
    txtRenderer.render(renderer, {hbox.x, hbox.y+dy, hbox.w, hbox.h}, m_input->c_str(),  m_input->length());
}

// is a modal layer, thus consumes all events
bool DialogLayer::hitTest(float, float) const noexcept {
    return true; 
}

void DialogLayer::onResponse(Responses&&) {}

void DialogLayer::processEvents(const KeyUpEvent& event) {
    // enter = finish the dialog 
    // backspace = delete last character
    if(event.key == SDLK_RETURN) {
        m_pendingRequest = !m_input->empty() 
            ? DialogCloseRequest{ DialogResponse{ std::move(*m_input) }}
            : DialogCloseRequest{ std::nullopt };
    } else if (event.key == SDLK_BACKSPACE) {
        if (!m_input->empty()) m_input->pop_back();
    }
}

void DialogLayer::processEvents(const TextInputEvent& event) {
    // append input text
    if (m_input && m_input->length() < m_maxInputLen)
        m_input->append(event.text);
}
