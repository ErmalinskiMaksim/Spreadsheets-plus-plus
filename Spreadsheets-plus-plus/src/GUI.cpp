#include "GUI.h"
#include "DepsInit.h"
// layers
#include "Layer.h"
// handlers
#include "TaskBarHandlers.h"
// interactors
#include "TaskBarInteractor.h"
#include "TableInteractor.h"
#include "ToolBarInteractor.h"
#include "MenuInteractor.h"
#include "DialogInteractor.h"
#include "PopupInteractor.h"

#include <iostream>

// Window data
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

GUI::GUI() : m_layers(3)
            , m_focusStack(1)
            , m_window{WINDOW_WIDTH, WINDOW_HEIGHT}
            , m_renderer{}
            , m_mainFont{}
{
    // initialize libraries
    if (!LibInitDeinit::init()) {
        std::cerr << "Error initializing the libraries: " << GetError() << std::endl;
    }

    if (!m_window.init()) {
        std::cerr << "Error creating Window: " <<  GetError() << std::endl;
        throw;
    }
    
    if (!m_renderer.init(m_window.get())) {
        std::cerr << "Error creating Renderer: " << GetError() << std::endl;
        throw;
    }
    
    if (!m_mainFont.init(m_renderer.get(), "fonts/Monoid-Regular.ttf", 16)) {
        std::cerr << "error opening main font: " << GetError() << std::endl;
        throw;
    }

    // initialize the layers in the ascending order
    auto charWidth = m_mainFont.getCharacterWidth();
    auto charHeight = m_mainFont.getCharacterHeight();
    m_layers[0] = std::make_unique
        <Layer<TableWidget, TableInteractor, NonModalLayerCreateRequest
        , TableOperationsActionHandler, TableCellActionHandler>>( 
            NonModalLayerCreateRequest{
                Widget {
                    Rect{0.0f, 0.2f * WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT * 0.8f}
                    , Color{0xEE, 0xEE, 0xEE, 0xFF}
                    , Color{0xEE, 0xEE, 0xEE, 0xFF}
                    , charWidth
                    , charHeight
                }, NonModalLayerCreateRequest::Payload{}}
            , TableOperationsActionHandler{}
            , TableCellActionHandler{}); // lowest layer
    m_layers[1] = std::make_unique
        <Layer<Widget, ToolBarInteractor, NonModalLayerCreateRequest>>(
            NonModalLayerCreateRequest{
                Widget {
                    Rect{0.0f, 0.05f * WINDOW_HEIGHT, WINDOW_WIDTH, 0.15f * WINDOW_HEIGHT}
                    , Color{0xBB, 0xBB, 0xBB, 0xFF}
                    , Color{0xBB, 0xBB, 0xBB, 0xFF}
                    , charWidth
                    , charHeight
                }, NonModalLayerCreateRequest::Payload{}}); 
    m_layers[2] = std::make_unique
        <Layer<Widget, TaskBarInteractor, NonModalLayerCreateRequest
        , FileActionHandler, HelpActionHandler>>(
            NonModalLayerCreateRequest{
                Widget {
                    Rect{0.0f, 0.0f, WINDOW_WIDTH, 0.05f * WINDOW_HEIGHT}, 
                    Color{0xCC, 0xCC, 0xCC, 0xFF},
                    Color{0xCC, 0xCC, 0xCC, 0xFF},
                    charWidth, charHeight
                }, NonModalLayerCreateRequest::Payload{}}
            , FileActionHandler{}
            , HelpActionHandler{});
    // by default the lowest layer has focus
    m_focusStack[0] = m_layers[0].get();
}

GUI::~GUI() {
    // destroy objects for clean lib deinit
    m_mainFont.destroy();
    m_renderer.destroy();
    m_window.destroy();
    // destroy libraries
    LibInitDeinit::deinit();
}

void GUI::setFocus(ILayer* layer) noexcept {
    // if not itself -> set focus 
    if (!m_focusStack.empty() && m_focusStack.back() == layer)
        return;
    m_focusStack.push_back(layer);
}

void GUI::popFocus() noexcept {
    if (!m_focusStack.empty())
        m_focusStack.pop_back();
}

ILayer* GUI::getFocusedLayer() const noexcept {
    return m_focusStack.empty() ? nullptr : m_focusStack.back();
}

bool GUI::processEvents() {
    // convert lib event to internal event
    auto guiEvent = translateEvent(waitEvent());
    if (!guiEvent) return true;
    
    return std::visit([&](auto&& ev) {
        using T = std::decay_t<decltype(ev)>;

        if constexpr (std::is_same_v<T, QuitEvent>)
            return false;
        // if event logically can propagate through layers
        else if constexpr (std::is_same_v<T, MouseLeftDownEvent>
                        || std::is_same_v<T, MouseLeftUpEvent>
                        || std::is_same_v<T, MouseRightUpEvent>
                        || std::is_same_v<T, MouseMotionEvent>) {
            // find a target layer that must consume the event and quit
            ILayer* target = nullptr;
            for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
                // event consumption is hit test based
                if ((*it)->hitTest(ev.x, ev.y)) {
                    // set focus to this layer
                    target = it->get();
                    setFocus(target);
                    // let the layer consume the event
                    target->dispatchEvents(ev);
                    break;  
                }
            }
        }// if an event should not logically propagate  
        else if constexpr (std::is_same_v<T, MouseScrollingEvent>
                        || std::is_same_v<T, TextInputEvent>
                        || std::is_same_v<T, KeyUpEvent>) {
            if (ILayer* layer = getFocusedLayer())
                layer->dispatchEvents(ev);
        }
        
        // requests can be chained
        while (processRequests());

        return true;
    }, *guiEvent);    
}

bool GUI::processRequests() {
    // fetch requests of the focused layer
    auto req = getFocusedLayer()->readRequest();
    if (!req) return false;
    
    // if layer requests destruction of itself
    if (std::holds_alternative<MenuCloseRequest>(*req)
        || std::holds_alternative<DialogCloseRequest>(*req)
        || std::holds_alternative<PopupCloseRequest>(*req)) {

        // destroy layer
        popFocus();
        m_layers.pop_back();

        // pass the final response of the destroyed layer to its caller
        std::visit([&](auto&& closeReq) {
            using T = std::decay_t<decltype(closeReq)>;
            if constexpr (std::is_same_v<T, MenuCloseRequest>
                    || std::is_same_v<T, DialogCloseRequest>
                    || std::is_same_v<T, PopupCloseRequest>) {
                if (closeReq.resp) 
                    getFocusedLayer()->onResponse(std::move(*closeReq.resp));
            }
        }, *req);

        // true == the layer that was given focus after the destruction of the
        // previous focused layer may want to immediately leave a new requests
        // after receiving previous layer's response, thus chain of requests.
        // It allows gui to react to events instantly
        return true;
    }

    // if a layer 
    if (auto* menuReq = std::get_if<MenuCreateRequest>(&*req)) {
        m_layers.push_back(std::make_unique
                <Layer<Widget, MenuInteractor, MenuCreateRequest>>(std::move(*menuReq)));
    } else if (auto* dialogReq = std::get_if<DialogCreateRequest>(&*req)) {
        m_layers.push_back(std::make_unique
                <Layer<Widget, DialogInteractor, DialogCreateRequest>>(std::move(*dialogReq)));
    } else if (auto* popupReq = std::get_if<PopupCreateRequest>(&*req)) {
        m_layers.push_back(std::make_unique
                <Layer<Widget, PopupInteractor, PopupCreateRequest>>(std::move(*popupReq)));
    } 
    // false == a new layer that was created first must process events before 
    // making requests, thus no chain of requests
    return false;
}

void GUI::draw() const {
    // draw main background
    m_renderer.clear(); 
    // let all the layers draw themselves in the down->top order
    for(auto&& layer : m_layers)
        layer->draw(m_renderer, m_mainFont);

    // present in a centralized maner
    m_renderer.present();
}
