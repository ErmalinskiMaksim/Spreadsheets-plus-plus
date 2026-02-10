#ifndef FONT_H
#define FONT_H

#include "DepsRendering.h"
#include <string_view>

#ifdef USE_SDL
class Font {
public:
    Font(RendererPtrType, std::string_view, float);
    Font(Font&&) = delete;
    Font& operator=(Font&&) = delete;

    void destroy();

    TexturePtrType getGlyphAtlas() const noexcept;  
    float getCharacterWidth() const noexcept;
    float getCharacterHeight() const noexcept;
private:
    bool prerenderGlyphAtlas(RendererPtrType);

    FontType m_font;
    TextureType m_glyphAtlas;
};
#elif USE_SFML

class Font {
public:
    Font(std::string_view, unsigned);

    float getCharacterWidth() const;
    float getCharacterHeight() const;
    unsigned size() const;
    FontViewType get() const; 

private:
    FontType m_font;
    unsigned m_fontSize;
};
#endif
#endif
