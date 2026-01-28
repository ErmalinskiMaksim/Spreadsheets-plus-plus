#ifndef POPUP_LAYER_H
#define POPUP_LAYER_H

#include "Layer.h"

// a modal layer that represents a pop up 
class PopupLayer final : public Layer {
    using TextLines = std::vector<std::string>;
public:
    PopupLayer(PopupCreateRequest&&);
    /// Layer's interface ///
    void dispatchEvents(const GuiEvent&) override;
    void draw(SDL_Renderer* const, const TextRenderer&) const override;
    void onResponse(Responses&&) noexcept override;
    bool hitTest(float, float) const noexcept override;
protected:
    // supported events
    using Layer::processEvents;
    void processEvents(const MouseLeftDownEvent&);

    // basic widget data
    Widget m_widget;
    // text content of a pop up
    TextLines m_text;
};

#endif
