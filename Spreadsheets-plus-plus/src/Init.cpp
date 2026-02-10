#include "Init.h"

// Custom handlers for layers
#include "TaskBarHandlers.h"

// Custom Interactors
#include "Interactors/TaskBarInteractor.h"
#include "TableInteractor.h"
#include "Interactors/ToolBarInteractor.h"

// Window
const std::string_view WINDOW_TITLE = "Spreadsheets++";
const unsigned WINDOW_WIDTH  = 800;
const unsigned WINDOW_HEIGHT = 600;

// Font
const std::string_view MAIN_FONT_PATH = "fonts/Monoid-Regular.ttf";
const float MAIN_FONT_SZ = 16;

const size_t MAIN_LAYER_COUNT = 3;
void initializeLayers(LayerArray& layers, float mainFontCharWidth, float mainFontCharHeight) {
    layers[0] = std::make_unique
        <Layer<TableWidget, TableInteractor, NonModalLayerCreateRequest
        , TableOperationsActionHandler, TableCellActionHandler>>( 
            NonModalLayerCreateRequest{
                Widget {
                    Rect{0.0f, 0.2f * WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT * 0.8f}
                    , Color{0xEE, 0xEE, 0xEE, 0xFF}
                    , Color{0xEE, 0xEE, 0xEE, 0xFF}
                    , mainFontCharWidth
                    , mainFontCharHeight
                }, NonModalLayerCreateRequest::Payload{}}
            , TableOperationsActionHandler{}
            , TableCellActionHandler{}); // lowest layer
    layers[1] = std::make_unique
        <Layer<Widget, ToolBarInteractor, NonModalLayerCreateRequest>>(
            NonModalLayerCreateRequest{
                Widget {
                    Rect{0.0f, 0.05f * WINDOW_HEIGHT, WINDOW_WIDTH, 0.15f * WINDOW_HEIGHT}
                    , Color{0xBB, 0xBB, 0xBB, 0xFF}
                    , Color{0xBB, 0xBB, 0xBB, 0xFF}
                    , mainFontCharWidth
                    , mainFontCharHeight
                }, NonModalLayerCreateRequest::Payload{}}); 
    layers[2] = std::make_unique
        <Layer<Widget, TaskBarInteractor, NonModalLayerCreateRequest
        , FileActionHandler, HelpActionHandler>>(
            NonModalLayerCreateRequest{
                Widget {
                    Rect{0.0f, 0.0f, WINDOW_WIDTH, 0.05f * WINDOW_HEIGHT}
                    , Color{0xCC, 0xCC, 0xCC, 0xFF}
                    , Color{0xCC, 0xCC, 0xCC, 0xFF}
                    , mainFontCharWidth
                    , mainFontCharHeight
                }, NonModalLayerCreateRequest::Payload{}}
            , FileActionHandler{}
            , HelpActionHandler{});
}
