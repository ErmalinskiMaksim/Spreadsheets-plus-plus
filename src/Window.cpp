#include "Window.h"

constexpr const char *WINDOW_TITLE = "Spreadsheets++";

Window::Window(int width, int height)
    : m_window{nullptr, SDL_DestroyWindow}
    , m_width(width)
    , m_height(height)
{}

bool Window::init() {
    m_window.reset(SDL_CreateWindow(WINDOW_TITLE, m_width, m_height, 0));
    if (!m_window) 
        return false; 
    
    // allow the typing
    SDL_StartTextInput(m_window.get());
    return true;
}

void Window::destroy() const {
    SDL_StopTextInput(m_window.get());
}

WindowPtrType Window::get() const noexcept {
    return m_window.get();
}
