#ifndef INIT_H
#define INIT_H 

// Layer 
#include "Layer.h"
#include <memory>
// Window data
constexpr std::string_view WINDOW_TITLE = "Spreadsheets++";
constexpr unsigned WINDOW_WIDTH  = 800;
constexpr unsigned WINDOW_HEIGHT = 600;

// Main font data
constexpr std::string_view MAIN_FONT_PATH = "fonts/Monoid-Regular.ttf";
constexpr float MAIN_FONT_SZ = 16;

// Main layers data
constexpr size_t MAIN_LAYER_COUNT = 3;

using LayerArray = std::vector<std::unique_ptr<ILayer>>;
void initializeLayers(LayerArray&, float, float);

#endif // !INIT_H
