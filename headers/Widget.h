#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_pixels.h>
#include <type_traits>
#include "TextRenderer.h"

class Widget;
template<typename T>
concept WidgetType = std::is_base_of_v<Widget, T>;

// color wrapper
struct Color {
    Color operator+(Uint8) const;
    Color operator-(Uint8) const;
    operator SDL_Color() const noexcept;
    Uint8 r = 0x00;
    Uint8 g = 0x00;
    Uint8 b = 0x00;
    Uint8 a = 0x00;
};

// Basic widget. 
// It contains bare-bone graphical information:
// * hitbox 
// * color 
// * font data
class Widget {
public:
    Widget(SDL_FRect&& hitBox = {0.0f, 0.0f, 0.0f, 0.0f}
           , Color fillColor = {0xFF, 0xFF, 0xFF, 0xFF}
           , Color outlineColor = {0x00, 0x00, 0x00, 0x00}
           , float charWidth = 0.0f, float charHeight = 0.0f
           );

    constexpr bool contains(float x, float y) const noexcept {
        return (x > m_hitBox.x && x <= m_hitBox.x + m_hitBox.w) 
            && (y > m_hitBox.y && y <= m_hitBox.y + m_hitBox.h);
    }

    static bool contains(float x, float y, const SDL_FRect& rect) noexcept {
        return (x > rect.x && x <= rect.x + rect.w) && (y > rect.y && y <= rect.y + rect.h); 
    }

    void render(SDL_Renderer*, const TextRenderer&) const noexcept;
    
    SDL_FRect getHitBox() const noexcept;
    Color getFillColor() const noexcept;
    Color getOutlineColor() const noexcept;
    float getCharWidth() const noexcept;
    float getCharHeight() const noexcept;
protected:
    // absolute coordinates
    SDL_FRect m_hitBox; 
    Color m_fillColor;
    Color m_outlineColor;
    float m_charWidth;
    float m_charHeight;
};

#endif // !WIDGET_H
