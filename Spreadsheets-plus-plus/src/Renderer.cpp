#include "Renderer.h"

Renderer::Renderer() 
    : m_renderer{nullptr, SDL_DestroyRenderer}
{}

bool Renderer::init(WindowPtrType window) {
    m_renderer.reset(SDL_CreateRenderer(window, nullptr));
    return static_cast<bool>(m_renderer); 
}

void Renderer::destroy() {
    m_renderer.reset();
}

void Renderer::renderLines(const Point* points, int count, Color4 lineColor) const {
    SDL_SetRenderDrawColor(m_renderer.get(), lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    SDL_RenderLines(m_renderer.get(), points, count);
}

void Renderer::renderFillRect(const Rect* const rect, Color4 fillColor) const {
    SDL_SetRenderDrawColor(m_renderer.get(), fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(m_renderer.get(), rect);
}

void Renderer::renderRect(const Rect* const rect, Color4 outlineColor) const {
    SDL_SetRenderDrawColor(m_renderer.get(), outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
    SDL_RenderRect(m_renderer.get(), rect);
}

void Renderer::renderText(const Font& font, Rect dest, std::string_view txt) const {
    auto w = font.getCharacterWidth();
    auto h = font.getCharacterHeight();
    SDL_FRect srcRect{0.0f, 0.0f, w, h};
    // draw each character string characters one by one by cutting out
    // characters from the glyph atlas
    float xLim = dest.x + dest.w;
    dest.w = w;
    dest.h = h;
    for(auto&& c : txt) {
        if (dest.x >= xLim) break;
        srcRect.x = w * static_cast<float>(c - ' ');
        SDL_RenderTexture(m_renderer.get(), font.getGlyphAtlas(), &srcRect, &dest);
        dest.x += w;
    }
}

void Renderer::clear() const {
    SDL_SetRenderDrawColor(m_renderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(m_renderer.get());
}

void Renderer::present() const {
    SDL_RenderPresent(m_renderer.get());
}

RendererPtrType Renderer::get() const noexcept {
    return m_renderer.get();
}
