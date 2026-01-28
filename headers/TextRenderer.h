#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H
#include <SDL3/SDL_render.h>
#include <memory>

struct TTF_Font;

// The main class that is responsible for text rendering logic
class TextRenderer {
public:
    TextRenderer();
    
    bool init(SDL_Renderer*, TTF_Font*);
    void render(SDL_Renderer* const, SDL_FRect, const char* const, size_t) const;    
    
    float getCharacterWidth() const noexcept;
    float getCharacterHeight() const noexcept;
private:
    // atlas of all characters in the application
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> m_glyphAtlas;
};

#endif 
