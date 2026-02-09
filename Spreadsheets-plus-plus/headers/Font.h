#ifndef FONT_H
#define FONT_H

#include "DepsRendering.h"
#include <string_view>

class Font {
public:
    Font();
    // Font(Font&&) = delete;
    // Font& operator=(Font&&) = delete;

    bool init(RendererPtrType, std::string_view, float);
    void destroy();

    TexturePtrType getGlyphAtlas() const noexcept;  
    float getCharacterWidth() const noexcept;
    float getCharacterHeight() const noexcept;
private:
    bool prerenderGlyphAtlas(RendererPtrType);

    FontType m_font;
    TextureType m_glyphAtlas;
};

#endif
