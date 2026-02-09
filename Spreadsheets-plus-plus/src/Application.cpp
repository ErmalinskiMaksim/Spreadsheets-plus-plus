#include "Application.h"
#include "GUI.h"

void Application::run() {
    while(processEvents()) display();
}

// Application currently supports only gui mode
bool Application::processEvents() {
   return GUI::get().processEvents();
}

void Application::display() {
    GUI::get().draw();
}
