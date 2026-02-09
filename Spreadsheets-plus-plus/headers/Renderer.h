#ifndef RENDERER_H
#define RENDERER_H

#include "Font.h"

class Renderer {
public:
    Renderer();
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    
    bool init(WindowPtrType);
    void destroy();

    // void renderLines
    void renderLines(const Point*, int, Color4) const;
    void renderFillRect(const Rect* const, Color4) const;
    void renderRect(const Rect* const, Color4) const;    
    void renderText(const Font&, Rect, std::string_view) const;

    void clear() const;
    void present() const;
    
    RendererPtrType get() const noexcept;
private: 
    RendererType m_renderer;
};

#endif
