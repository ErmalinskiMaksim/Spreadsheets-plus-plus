#ifndef WINDOW_H
#define WINDOW_H

#include "DepsRendering.h"

class Window {
public:
    Window(int, int);
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool init();
    void destroy() const;

    WindowPtrType get() const noexcept;
private:
    WindowType m_window;
    int m_width;
    int  m_height;
};

#endif
