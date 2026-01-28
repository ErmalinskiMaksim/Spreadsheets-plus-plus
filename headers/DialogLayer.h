#ifndef DIALOG_LAYER_H
#define DIALOG_LAYER_H

#include "Layer.h"

// a modal layer that is responsible for dialogs
class DialogLayer final : public Layer {
public:
    DialogLayer(DialogCreateRequest&&);
    //// Layer's interface ////
    void dispatchEvents(const GuiEvent&) override;
    void draw(SDL_Renderer* const, const TextRenderer&) const override;
    void onResponse(Responses&&) override;
    bool hitTest(float, float) const noexcept override;
protected:
    // Supported events
    using Layer::processEvents;
    void processEvents(const KeyUpEvent&);
    void processEvents(const TextInputEvent&);

    // title of the dialog (may or may not be)
    std::optional<std::string> m_title;
    // input text (can either be filled at first or empty)
    std::optional<std::string> m_input;
    // basic widget data
    Widget m_widget;
    // maximum input lenght
    uint8_t m_maxInputLen;
};

#endif // !DEBUG
