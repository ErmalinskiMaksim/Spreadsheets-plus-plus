#include "Renderer.h"

#ifdef USE_SDL
#include <iostream>

Renderer::Renderer(WindowPtrType window) 
    : m_renderer{SDL_CreateRenderer(window, nullptr), SDL_DestroyRenderer}
{
    if (!m_renderer) {
        std::cerr << "Error creating Renderer: " << SDL_GetError() << std::endl;
        throw;
    }
}

void Renderer::destroy() {
    m_renderer.reset();
}

RendererPtrType Renderer::get() const noexcept {
    return m_renderer.get();
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
    Rect srcRect{0.0f, 0.0f, w, h};
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

#elif USE_SFML

Rect::operator FRect() const noexcept {
    return FRect{x, y, w, h};
}


#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

Renderer::Renderer(std::string_view title, unsigned width, unsigned height) 
    : m_renderer{sf::VideoMode{width, height}, std::string{title}, sf::Style::Titlebar | sf::Style::Close} 
{
    // Get desktop resolution
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    m_renderer.setPosition(sf::Vector2i(
        (desktop.width - width) / 2,
        (desktop.height - height) / 2
    ));
}


RendererViewType Renderer::get() const noexcept {
    return m_renderer;
}

void Renderer::renderLines(const Point* points, int count, Color4 lineColor) const {
    sf::VertexArray lines(sf::LineStrip, static_cast<size_t>(count));
    
    for (auto i = 0uz; i < static_cast<size_t>(count); ++i) {
        lines[i].position = sf::Vector2f(points[i].x, points[i].y);
        lines[i].color = lineColor;
    }
    m_renderer.draw(lines);
}

void Renderer::renderFillRect(const Rect* const rect, Color4 fillColor) const {
    sf::RectangleShape shape(sf::Vector2f(rect->w, rect->h));
    shape.setPosition(rect->x, rect->y);
    shape.setFillColor(fillColor);
    m_renderer.draw(shape);
}

void Renderer::renderRect(const Rect* const rect, Color4 outlineColor) const {
    sf::RectangleShape shape(sf::Vector2f(rect->w, rect->h));
    shape.setPosition(rect->x, rect->y);
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(1.0f); 
    m_renderer.draw(shape);
}

void Renderer::renderText(const Font& font, Rect dest, std::string_view txt) const {
    sf::Text text;
    text.setFont(font.get());
    text.setString(std::string(txt)); // FIXME 
    text.setCharacterSize(font.size()); 
    text.setPosition(dest.x, dest.y);
    text.setFillColor(sf::Color::Black);

    m_renderer.draw(text);
}

void Renderer::clear() const {
    m_renderer.clear(sf::Color::White);
}

void Renderer::present() const {
    m_renderer.display();
}

#endif
