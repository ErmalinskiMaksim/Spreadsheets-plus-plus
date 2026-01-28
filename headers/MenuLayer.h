#ifndef MENU_LAYER_H 
#define MENU_LAYER_H

#include "Layer.h"

struct MenuCreateRequest;

// a modal layer that represents a menu
class MenuLayer final : public Layer {
    using Entries = std::vector<MenuCreateRequest::MenuAction>;
public:
    MenuLayer(MenuCreateRequest&&);
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
    // menu entries that consist of text and action ID
    Entries m_entries; 
};

#endif 
