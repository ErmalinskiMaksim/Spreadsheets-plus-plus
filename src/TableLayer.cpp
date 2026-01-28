#include "TableLayer.h"

TableLayer::TableLayer(Widget&& widget) 
    : Layer()
    , m_tableWidget{std::move(widget)}
    , m_interactor{std::ref(m_tableWidget), std::ref(m_pendingRequest)}
{}

void TableLayer::dispatchEvents(const GuiEvent& event) {
    std::visit([&](auto&& ev) { m_interactor.processEvents(ev); }, event);
}

void TableLayer::onResponse(Responses&& resp) {
    m_interactor.dispatchResponse(std::forward<Responses>(resp));
}

void TableLayer::draw(SDL_Renderer* const renderer, const TextRenderer& textRenderer) const {
    m_tableWidget.render(renderer, textRenderer);
    m_interactor.render(renderer, textRenderer);
} 

bool TableLayer::hitTest(float x, float y) const noexcept {
    return m_tableWidget.contains(x, y);
} 
