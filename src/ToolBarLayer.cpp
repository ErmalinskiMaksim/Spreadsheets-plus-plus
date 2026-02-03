#include "ToolBarLayer.h"
#include <SDL3/SDL_render.h>

ToolBarLayer::ToolBarLayer(Widget&& widget) 
    : m_widget(std::move(widget))
{}

void ToolBarLayer::dispatchEvents(const GuiEvent&) {
    // std::visit([&](auto&& ev) { processEvents(ev); }, event);
}

void ToolBarLayer::draw(SDL_Renderer* const renderer, const TextRenderer& textRenderer) const {
    m_widget.render(renderer, textRenderer);
}

void ToolBarLayer::onResponse(Responses&&) {}

bool ToolBarLayer::hitTest(float x, float y) const noexcept { 
    return m_widget.contains(x, y);
}
