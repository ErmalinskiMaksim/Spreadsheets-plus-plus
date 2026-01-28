#ifndef TOOL_BAR_LAYER_H
#define TOOL_BAR_LAYER_H

#include "Layer.h"

class ToolBarLayer final : public Layer {
public:
    ToolBarLayer(Widget&&);
    /// Layer's interface ///
    void dispatchEvents(const GuiEvent&) override;
    void draw(SDL_Renderer* const, const TextRenderer&) const override;
    void onResponse(Responses&&) override;
    bool hitTest(float, float) const noexcept override;
protected:
    // supported evetns
    // using Layer::processEvents;

    // basic widget data
    Widget m_widget;
};
#endif  
