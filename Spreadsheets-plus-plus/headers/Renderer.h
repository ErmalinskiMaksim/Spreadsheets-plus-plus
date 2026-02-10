#ifndef RENDERER_H
#define RENDERER_H

#include "Font.h"

#ifdef USE_SDL
class Renderer {
public:
    Renderer(WindowPtrType);
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void destroy();
    RendererPtrType get() const noexcept;

    void renderLines(const Point*, int, Color4) const;
    void renderFillRect(const Rect* const, Color4) const;
    void renderRect(const Rect* const, Color4) const;    
    void renderText(const Font&, Rect, std::string_view) const;

    void clear() const;
    void present() const;
    
private: 
    mutable RendererType m_renderer;
};

#elif USE_SFML
struct Rect {
    operator FRect() const noexcept;
    float x;
    float y;
    float w;
    float h;
};


class Renderer {
public:
    Renderer(std::string_view, unsigned, unsigned);
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    RendererViewType get() const noexcept;

    void renderLines(const Point*, int, Color4) const;
    void renderFillRect(const Rect* const, Color4) const;
    void renderRect(const Rect* const, Color4) const;    
    void renderText(const Font&, Rect, std::string_view) const;

    void clear() const;
    void present() const;
    
private: 
    mutable RendererType m_renderer;
};


#endif

#endif
