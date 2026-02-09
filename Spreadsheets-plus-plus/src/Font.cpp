#include "Font.h"

Font::Font()
    : m_font{nullptr, TTF_CloseFont}
    , m_glyphAtlas{nullptr, SDL_DestroyTexture}
{}

bool Font::init(RendererPtrType renderer, std::string_view path, float sz) {
    m_font.reset(TTF_OpenFont(path.data(), sz));
    if (!m_font) return false;

    return prerenderGlyphAtlas(renderer);
}

void Font::destroy() {
    m_font.reset();
}

bool Font::prerenderGlyphAtlas(RendererPtrType renderer) {
    // create a string of ASCII symbols 32 through 95
    char atlas[96];
    for(int i = 0; i < 95; ++i) atlas[i] = static_cast<char>(i + 32);
    atlas[95] = '\0';

    // create a texture surface to draw the glyph atlas on
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> textSurf = {
        TTF_RenderText_Blended(m_font.get(), atlas, 0, {0x00, 0x00, 0x00, 0xff}),
        SDL_DestroySurface
    };
    if(!textSurf) return false;

    m_glyphAtlas.reset(SDL_CreateTextureFromSurface(renderer, textSurf.get()));
    return static_cast<bool>(m_glyphAtlas);
}

TexturePtrType Font::getGlyphAtlas() const noexcept {
    return m_glyphAtlas.get();
}

float Font::getCharacterWidth() const noexcept {
    return (m_glyphAtlas) ? static_cast<float>(m_glyphAtlas->w) / 95 : 0.0f;
}

float Font::getCharacterHeight() const noexcept {
    return (m_glyphAtlas) ? static_cast<float>(m_glyphAtlas->h) : 0.0f; 
}
