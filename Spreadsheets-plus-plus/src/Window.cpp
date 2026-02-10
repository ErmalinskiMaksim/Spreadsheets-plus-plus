#ifdef USE_SDL
#include "Window.h"
#include <iostream>

Window::Window(std::string_view title, int width, int height)
    : m_window{SDL_CreateWindow(title.data(), width, height, 0), SDL_DestroyWindow}
{
    if (!m_window) {
        std::cerr << "Error creating Window: " << SDL_GetError() << std::endl;
        throw;
    }

    // allow the typing
    SDL_StartTextInput(m_window.get());
}

void Window::destroy() const {
    SDL_StopTextInput(m_window.get());
}

WindowPtrType Window::get() const noexcept {
    return m_window.get();
}
#endif
