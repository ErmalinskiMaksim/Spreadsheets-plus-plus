#include "PopupLayer.h"
#include "TextRenderer.h"

PopupLayer::PopupLayer(PopupCreateRequest&& req)
    : m_widget(std::move(req.widget)) 
    , m_text(std::move(req.lines))
{}

void PopupLayer::dispatchEvents(const GuiEvent& event) {
    std::visit([&](auto&& ev) { processEvents(ev); }, event);
}

void PopupLayer::draw(SDL_Renderer* const renderer, const TextRenderer& textRenderer) const {
    auto hbox = m_widget.getHitBox();
    auto c = m_widget.getColor();
    
    // draw pop up background
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &hbox);

    // draw pop up outline 
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderRect(renderer, &hbox);

    // draw text content of the pop up
    auto dy = textRenderer.getCharacterHeight();
    for (auto i = 0uz; i < m_text.size(); ++i) 
        textRenderer.render(renderer, { hbox.x, hbox.y + static_cast<float>(i)*dy, hbox.w, dy}
                , m_text[i].c_str(), m_text[i].length());
}

void PopupLayer::onResponse(Responses&&) noexcept {}

// is a modal layer, thus consumes all events
bool PopupLayer::hitTest(float, float) const noexcept {
    return true;
}

void PopupLayer::processEvents(const MouseLeftDownEvent& event) {
    // on LMB click request todestroy itself
    if (m_widget.contains(event.x, event.y))
        m_pendingRequest = PopupCloseRequest{};
}

