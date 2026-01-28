#include "MenuLayer.h"
#include "TextRenderer.h"

MenuLayer::MenuLayer(MenuCreateRequest&& req)
    : m_widget(std::move(req.widget))
    , m_entries(std::move(req.entries))
{}

void MenuLayer::dispatchEvents(const GuiEvent& event) {
    std::visit([&](auto&& ev) { processEvents(ev); }, event);
}

void MenuLayer::onResponse(Responses&&) noexcept {}

// is a modal layer, thus consumes all events
bool MenuLayer::hitTest(float, float) const noexcept { 
    return true; 
}

void MenuLayer::draw(SDL_Renderer* const renderer, const TextRenderer& textRenderer) const {
    auto hbox = m_widget.getHitBox();
    auto c = m_widget.getColor();

    // draw menu background
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &hbox);

    // draw all the menu entries
    auto dy = textRenderer.getCharacterHeight();
    for (auto i = 0uz; i < m_entries.size(); ++i) 
        textRenderer.render(renderer, { hbox.x, hbox.y + static_cast<float>(i)*dy, hbox.w, dy}
                , m_entries[i].text.c_str(), m_entries[i].text.length());
}

void MenuLayer::processEvents(const MouseLeftDownEvent& event) {
    // if menu widget was clicked, get action id and send a request to destroy itself
    if (m_widget.contains(event.x, event.y)) {
        auto hbox = m_widget.getHitBox();
        auto idx = static_cast<size_t>((event.y - hbox.y) * static_cast<float>(m_entries.size()) / hbox.h);
        m_pendingRequest = MenuCloseRequest{ MenuResponse{m_entries[idx].id} };
    } // if a click happened outside the widget assume that no action was chosen 
      // ans similarly send a close request
    else m_pendingRequest = MenuCloseRequest{ std::nullopt };
}
