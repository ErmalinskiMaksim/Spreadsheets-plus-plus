#ifndef GUI_H
#define GUI_H
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextRenderer.h"
#include <vector>

class ILayer;

// The main GUI manager class 
// * initializes and destroys SDL and SDL_TTF
// * owns windows, renderers, fonts and graphical layers
// * dispatches events to layers in a centralized maner 
// * allows for communication between layers via requests&responses
// * deals with layer stack and determines focus
class GUI {
public:
    // singleton
    GUI(const GUI&) = delete;
    GUI(GUI&&) = delete;
    GUI& operator=(const GUI&) = delete;
    GUI& operator=(GUI&&) = delete;
    ~GUI();

    static GUI& get() {
        static GUI gui;
        return gui;
    }

    // process application events and dispatch them to layers 
    bool processEvents();
    // let all the layers draw themselves
    void draw() const;
private:
    GUI();
    
    // process layer requests
    bool processRequests();
    
    // layer focus handling
    void setFocus(ILayer*) noexcept;
    void popFocus() noexcept;
    ILayer* getFocusedLayer() const noexcept;
    
    // z-depth array of layers [bottom->top]
    std::vector<std::unique_ptr<ILayer>> m_layers; 
    // non owning focus stack of layers
    std::vector<ILayer*> m_focusStack; 
    // main window
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window;
    // main renderer
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> m_renderer;
    // main font
    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> m_mainFont;
    // main text renderer
    TextRenderer m_textRenderer;
};

#endif // !GUI_H
