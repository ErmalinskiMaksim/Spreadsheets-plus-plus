#include "Application.h"
#include <iostream>

int main() {
    int exit_val = EXIT_SUCCESS;

    try {
        Application app;
        app.run();
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        exit_val = EXIT_FAILURE;
    }

    return exit_val;
}
