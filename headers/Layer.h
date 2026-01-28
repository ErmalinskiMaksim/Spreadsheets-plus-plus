#ifndef LAYER_H
#define LAYER_H
#include "Events.h"
#include "Requests.h"

struct SDL_Renderer;
class TextRenderer;

// Layer interface. Represents a graphical layer. 
// * posses widgets and interactors
// * dispatches and processes events delivered by GUI
// * draws its widgets
class Layer {
public:
    // dispatch events passed from GUI
    virtual void dispatchEvents(const GuiEvent&) = 0;
    // draw widgets
    virtual void draw(SDL_Renderer* const, const TextRenderer&) const = 0;
    // react to responses to requests
    virtual void onResponse(Responses&&) = 0;
    // * test if mouse event is directed to the layer
    // * it may be more complex then just checking if 
    // the mouse is contained within a hit box:
    // e.g. modal layers that always consume hits
    virtual bool hitTest(float, float) const noexcept = 0;
    virtual ~Layer() = default;

    // allow GUI read layer's requests
    OptRequest readRequest();
protected:
    // process events of individual types
    template<GuiEventType Event>
    inline void processEvents(const Event&) noexcept {}

    // requests to the GUI
    OptRequest m_pendingRequest;
};
#endif
