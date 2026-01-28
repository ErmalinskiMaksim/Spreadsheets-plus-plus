#include "TextRenderer.h"
#include <SDL3_ttf/SDL_ttf.h>

TextRenderer::TextRenderer() 
    : m_glyphAtlas(nullptr, SDL_DestroyTexture) {}

// initialize a glyph atlas
bool TextRenderer::init(SDL_Renderer* renderer, TTF_Font* font) {
    // create a string of ASCII symbols 32 through 95
    char atlas[96];
    for(int i = 0; i < 95; ++i) atlas[i] = static_cast<char>(i + 32);
    atlas[95] = '\0';

    // create a texture surface to draw the glyph atlas on
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> textSurf = {
        TTF_RenderText_Blended(font, atlas, 0, {0x00, 0x00, 0x00, 0xff}),
        SDL_DestroySurface
    };
    if(!textSurf) return false;

    m_glyphAtlas.reset(SDL_CreateTextureFromSurface(renderer, textSurf.get()));
    if(!m_glyphAtlas) return false;
    
    return true;
}

void TextRenderer::render(SDL_Renderer* const renderer, SDL_FRect dest, const char* const cstr, size_t strlen) const {
    auto w = getCharacterWidth();
    auto h = getCharacterHeight();
    SDL_FRect srcRect{0.0f, 0.0f, w, h};
    // draw each character string characters one by one by cutting out
    // characters from the glyph atlas
    float xLim = dest.x + dest.w;
    dest.w = w;
    dest.h = h;
    for(size_t i = 0; i < strlen; ++i) {
        if (dest.x >= xLim) break;
        srcRect.x = w * static_cast<float>(cstr[i] - ' ');
        SDL_RenderTexture(renderer, m_glyphAtlas.get(), &srcRect, &dest);
        dest.x += w;
    }
}

float TextRenderer::getCharacterWidth() const noexcept {
    return static_cast<float>(m_glyphAtlas->w) / 95;
}

float TextRenderer::getCharacterHeight() const noexcept {
    return static_cast<float>(m_glyphAtlas->h);
}
