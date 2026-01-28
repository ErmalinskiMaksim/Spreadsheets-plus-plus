#ifndef WIDGET_H
#define WIDGET_H

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_pixels.h>

// color wrapper
struct Color {
    Color operator+(Uint8) const;
    Color operator-(Uint8) const;
    operator SDL_Color() const noexcept;
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
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
           , float charWidth = 0.0f, float charHeight = 0.0f
           );

    bool contains(float, float) const noexcept;
    static bool contains(float x, float y, const SDL_FRect& rect) noexcept {
        return (x > rect.x && x <= rect.x + rect.w) && (y > rect.y && y <= rect.y + rect.h); 
    }
    
    SDL_FRect getHitBox() const noexcept;
    Color getColor() const noexcept;
    float getCharWidth() const noexcept;
    float getCharHeight() const noexcept;
protected:
    // absolute coordinates
    SDL_FRect m_hitBox; 
    Color m_fillColor;
    float m_charWidth;
    float m_charHeight;
};

#endif // !WIDGET_H
