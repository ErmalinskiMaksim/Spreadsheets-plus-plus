#ifdef USE_SDL
#ifndef WINDOW_H
#define WINDOW_H

#include "DepsRendering.h"

class Window {
public:
    Window(std::string_view, int, int);
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    void destroy() const;

    WindowPtrType get() const noexcept;
private:
    WindowType m_window;
    std::string_view m_title;
    int m_width;
    int m_height;
};
#endif
#endif
