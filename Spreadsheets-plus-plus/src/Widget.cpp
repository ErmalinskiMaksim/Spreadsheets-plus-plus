#include "Widget.h"

Color Color::operator+(Color1 delta) const {
    return Color(r + delta, delta + g, delta + b, a);
}

Color Color::operator-(Color1 delta) const {
    return Color(r - delta, g - delta, b - delta, a);
}

Color::operator Color4() const noexcept {
    return Color4(r, g, b, a);
}

Widget::Widget(Rect&& hitBox, Color fillColor, Color outlineColor, float charWidth, float charHeight) 
    : m_hitBox(hitBox)
      , m_fillColor(fillColor)
      , m_outlineColor(outlineColor)
      , m_charWidth(charWidth)
      , m_charHeight(charHeight)
{}

// constexpr bool Widget::contains(float x, float y) const noexcept {
//     return (x > m_hitBox.x && x <= m_hitBox.x + m_hitBox.w) 
//         && (y > m_hitBox.y && y <= m_hitBox.y + m_hitBox.h);
// }

void Widget::render(const Renderer& renderer, const Font&) const noexcept {
    renderer.renderFillRect(&m_hitBox, m_fillColor);
    renderer.renderRect(&m_hitBox, m_outlineColor);
}

Rect Widget::getHitBox() const noexcept {
    return m_hitBox;
}

Color Widget::getFillColor() const noexcept {
    return m_fillColor;
}

Color Widget::getOutlineColor() const noexcept {
    return m_outlineColor;
}

float Widget::getCharWidth() const noexcept {
    return m_charWidth;
}

float Widget::getCharHeight() const noexcept {
    return m_charHeight;
}

