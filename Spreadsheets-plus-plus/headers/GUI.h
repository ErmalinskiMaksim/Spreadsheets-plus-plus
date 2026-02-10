#ifndef GUI_H
#define GUI_H
#include "Renderer.h"
#include "Window.h"
#include <vector>
#include <memory>

class ILayer;

// The main GUI manager class 
// * initializes and destroys libraries 
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

    static bool init() {
        #ifdef USE_SDL 
            constexpr SDL_InitFlags SDL_FLAGS = SDL_INIT_VIDEO;         
            return SDL_Init(SDL_FLAGS) && TTF_Init();
        #elif USE_SFML 
            return true;  
        #else
            return false;
        #endif
    }

    // process application events and dispatch them to layers 
    bool processEvents();
    // let all the layers draw themselves
    void draw() const;
private:
    GUI();
    
    static void deinit() {
        #ifdef USE_SDL
            TTF_Quit();
            SDL_Quit();
        #endif
    }

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
    #ifdef USE_SDL
    // main window
    Window m_window;
    #endif // USE_SDL

    // main renderer
    Renderer m_renderer;
    // main font
    Font m_mainFont;
};

#endif // !GUI_H
