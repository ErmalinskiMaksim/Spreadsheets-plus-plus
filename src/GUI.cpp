#include "GUI.h"
// layers
#include "ILayer.h"
#include "Layer.h"
// handlers
#include "Requests.h"
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
constexpr SDL_InitFlags SDL_FLAGS = SDL_INIT_VIDEO;
constexpr const char *WINDOW_TITLE = "Spreadsheet";

GUI::GUI() : m_layers(3)
            , m_focusStack(1)
            , m_window{nullptr, SDL_DestroyWindow}
            , m_renderer{nullptr, SDL_DestroyRenderer}
            , m_mainFont{nullptr, TTF_CloseFont}
            , m_textRenderer{}
{
    // initialize libraries
    if (!SDL_Init(SDL_FLAGS)) {
        std::cerr << "Error initializing SDL3: " << SDL_GetError() << std::endl;
        throw;
    }
    
    if (!TTF_Init()) {
        std::cerr << "Error initializing SDL3_ttf: " << SDL_GetError() << std::endl;
        throw;
    }

    // initialize library objects
    m_window.reset(SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0));
    if (!m_window) {
        std::cerr << "Error creating Window: " <<  SDL_GetError() << std::endl;
        throw;
    }

    m_renderer.reset(SDL_CreateRenderer(m_window.get(), nullptr));
    if (!m_renderer) {
        std::cerr << "Error creating Renderer: " << SDL_GetError() << std::endl;
        throw;
    }

    m_mainFont.reset(TTF_OpenFont("fonts/Monoid-Regular.ttf", 16));
    if (!m_mainFont) {
        std::cerr << "error opening main font: " << SDL_GetError() << std::endl;
        throw;
    }

    m_textRenderer.init(m_renderer.get(), m_mainFont.get());

    // allow the typing
    SDL_StartTextInput(m_window.get());

    // initialize the layers in the ascending order
    auto charWidth = m_textRenderer.getCharacterWidth();
    auto charHeight = m_textRenderer.getCharacterHeight();
    m_layers[0] = std::make_unique
        <Layer<TableWidget, TableInteractor, NonModalLayerCreateRequest, false
        , TableOperationsActionHandler, TableCellActionHandler>>( 
            NonModalLayerCreateRequest{
                Widget {
                    SDL_FRect{0.0f, 0.2f * WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT * 0.8f}
                    , Color{0xEE, 0xEE, 0xEE, 0xFF}
                    , Color{0xEE, 0xEE, 0xEE, 0xFF}
                    , charWidth
                    , charHeight
                }, NonModalLayerCreateRequest::Payload{}}
            , TableOperationsActionHandler{}
            , TableCellActionHandler{}); // lowest layer
    m_layers[1] = std::make_unique
        <Layer<Widget, ToolBarInteractor, NonModalLayerCreateRequest, false>>(
            NonModalLayerCreateRequest{
                Widget {
                    SDL_FRect{0.0f, 0.05f * WINDOW_HEIGHT, WINDOW_WIDTH, 0.15f * WINDOW_HEIGHT}
                    , Color{0xBB, 0xBB, 0xBB, 0xFF}
                    , Color{0xBB, 0xBB, 0xBB, 0xFF}
                    , charWidth
                    , charHeight
                }, NonModalLayerCreateRequest::Payload{}}); 
    m_layers[2] = std::make_unique
        <Layer<Widget, TaskBarInteractor, NonModalLayerCreateRequest, false
        , FileActionHandler, HelpActionHandler>>(
            NonModalLayerCreateRequest{
                Widget {
                    SDL_FRect{0.0f, 0.0f, WINDOW_WIDTH, 0.05f * WINDOW_HEIGHT}, 
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
    SDL_StopTextInput(m_window.get());
    // font must be destroyed before TTF_Quit
    m_mainFont.reset();
    // destroy library
    TTF_Quit();
    // sdl objects must be destroyed before SDL_Quit
    m_renderer.reset();
    m_window.reset();
    // destroy library
    SDL_Quit();
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
    // wait for SDL events
    SDL_Event SDLEvent;
    SDL_WaitEvent(&SDLEvent);
    
    // convert SDL event to internal event
    if (auto guiEvent = translateEvent(SDLEvent)) {
        // if quit -> return immediately 
        if (std::holds_alternative<QuitEvent>(*guiEvent))
            return false;
        // if event logically can propagate through layers
        else if (std::holds_alternative<MouseLeftUpEvent>(*guiEvent)
                || std::holds_alternative<MouseLeftDownEvent>(*guiEvent)
                || std::holds_alternative<MouseRightUpEvent>(*guiEvent)
                || std::holds_alternative<MouseMotionEvent>(*guiEvent)) { 
            std::visit([&](auto&& ev){
                using T = std::decay_t<decltype(ev)>;
                if constexpr (std::is_same_v<T, MouseLeftUpEvent> 
                        || std::is_same_v<T, MouseLeftDownEvent>
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
                            target->dispatchEvents(*guiEvent);
                            break;  
                        }
                    }
                }
            }, *guiEvent);
        } // if an event should not logically propagate and should 
          // always be consumed by the focused layer
        else if (std::holds_alternative<KeyUpEvent>(*guiEvent)
                || std::holds_alternative<TextInputEvent>(*guiEvent)
                || std::holds_alternative<MouseScrollingEvent>(*guiEvent)) {
            if (ILayer* layer = getFocusedLayer())
                layer->dispatchEvents(*guiEvent);
        }
    }

    // requests can be chained
    while (processRequests());
    return true;
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
                <Layer<Widget, MenuInteractor, MenuCreateRequest, true>>(std::move(*menuReq)));
    } else if (auto* dialogReq = std::get_if<DialogCreateRequest>(&*req)) {
        m_layers.push_back(std::make_unique
                <Layer<Widget, DialogInteractor, DialogCreateRequest, true>>(std::move(*dialogReq)));
    } else if (auto* popupReq = std::get_if<PopupCreateRequest>(&*req)) {
        m_layers.push_back(std::make_unique
                <Layer<Widget, PopupInteractor, PopupCreateRequest, true>>(std::move(*popupReq)));
    } 
    // false == a new layer that was created first must process events before 
    // making requests, thus no chain of requests
    return false;
}

void GUI::draw() const {
    // draw main background
    SDL_SetRenderDrawColor(m_renderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(m_renderer.get());
    
    // let all the layers draw themselves in the down->top order
    for(auto&& layer : m_layers)
        layer->draw(m_renderer.get(), m_textRenderer);

    // present in a centralized maner
    SDL_RenderPresent(m_renderer.get());
}
