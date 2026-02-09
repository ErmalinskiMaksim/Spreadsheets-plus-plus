#ifndef WIDGET_H
#define WIDGET_H

#include <type_traits>
#include "Renderer.h"

class Widget;
template<typename T>
concept WidgetType = std::is_base_of_v<Widget, T>;

// color wrapper
struct Color {
    Color operator+(Color1) const;
    Color operator-(Color1) const;
    operator Color4() const noexcept;
    Color1 r = 0x00;
    Color1 g = 0x00;
    Color1 b = 0x00;
    Color1 a = 0x00;
};

// Basic widget. 
// It contains bare-bone graphical information:
// * hitbox 
// * color 
// * font data
class Widget {
public:
    Widget(Rect&& hitBox = {0.0f, 0.0f, 0.0f, 0.0f}
           , Color fillColor = {0xFF, 0xFF, 0xFF, 0xFF}
           , Color outlineColor = {0x00, 0x00, 0x00, 0x00}
           , float charWidth = 0.0f, float charHeight = 0.0f
           );

    constexpr bool contains(float x, float y) const noexcept {
        return (x > m_hitBox.x && x <= m_hitBox.x + m_hitBox.w) 
            && (y > m_hitBox.y && y <= m_hitBox.y + m_hitBox.h);
    }

    static bool contains(float x, float y, const Rect& rect) noexcept {
        return (x > rect.x && x <= rect.x + rect.w) && (y > rect.y && y <= rect.y + rect.h); 
    }

    void render(const Renderer&, const Font&) const noexcept;
    
    Rect getHitBox() const noexcept;
    Color getFillColor() const noexcept;
    Color getOutlineColor() const noexcept;
    float getCharWidth() const noexcept;
    float getCharHeight() const noexcept;
protected:
    // absolute coordinates
    Rect m_hitBox; 
    Color m_fillColor;
    Color m_outlineColor;
    float m_charWidth;
    float m_charHeight;
};

#endif // !WIDGET_H
