#include "Widget.h"

Color Color::operator+(Uint8 delta) const {
    return Color(r + delta, delta + g, delta + b, a);
}

Color Color::operator-(Uint8 delta) const {
    return Color(r - delta, g - delta, b - delta, a);
}

Color::operator SDL_Color() const noexcept {
    return SDL_Color(r, g, b, a);
}

Widget::Widget(SDL_FRect&& hitBox, Color fillColor, float charWidth, float charHeight) 
    : m_hitBox(hitBox)
      , m_fillColor(fillColor)
      , m_charWidth(charWidth)
      , m_charHeight(charHeight)
{}

bool Widget::contains(float x, float y) const noexcept {
    return (x > m_hitBox.x && x <= m_hitBox.x + m_hitBox.w) 
        && (y > m_hitBox.y && y <= m_hitBox.y + m_hitBox.h);
}

SDL_FRect Widget::getHitBox() const noexcept {
    return m_hitBox;
}

Color Widget::getColor() const noexcept {
    return m_fillColor;
}

float Widget::getCharWidth() const noexcept {
    return m_charWidth;
}

float Widget::getCharHeight() const noexcept {
    return m_charHeight;
}

