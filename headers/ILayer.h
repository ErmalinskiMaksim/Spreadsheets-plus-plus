#ifndef ILAYER_H
#define ILAYER_H

#include "Events.h"
#include "Renderer.h"
#include "Requests.h"

// Layer interface. Represents a graphical layer. 
// * posses widgets and interactors
// * dispatches and processes events delivered by GUI
// * draws its widgets
class ILayer {
public:
    // dispatch events passed from GUI
    virtual void dispatchEvents(const LayerEvent&) = 0;
    // draw widgets
    virtual void draw(const Renderer&, const Font&) const = 0;
    // react to responses to requests
    virtual void onResponse(Responses&&) = 0;
    // * test if mouse event is directed to the layer
    // * it may be more complex then just checking if 
    // the mouse is contained within a hit box:
    // e.g. modal layers that always consume hits
    virtual bool hitTest(float, float) const noexcept = 0;
    virtual ~ILayer() = default;

    // allow GUI read layer's requests
    inline OptRequest readRequest() {
        auto req = std::move(m_pendingRequest);
        m_pendingRequest.reset();
        return req;
    }
protected:
    // requests to the GUI
    OptRequest m_pendingRequest;
};

#endif
