#include "TableStateMachine.h"
#include "DataStorage.h"
#include "TableWidget.h"
#include "TableInteractor.h"
#include "TableHandlers.h"
#include "SDL3/SDL_render.h"

OptState IdleTableState::process(const MouseLeftDownEvent& event) {
    auto& c = m_context.get();
    const auto& w = c.r_widget.get();

    c.m_mousePos = { event.x, event.y };
    if (SDL_GetModState() & SDL_KMOD_CTRL) {
        if (w.columnSpaceContains(c.m_mousePos))
            return ColumnResizingTableState{c}; // enter resizing state
        else if (w.rowSpaceContains(c.m_mousePos))
            return RowResizingTableState{c}; // enter resizing state
    } else if (w.spreadsheetSpaceContains(c.m_mousePos)) {
        // a click on the cell -> request a dialog to type cell's content
        auto [col, row] = w.getSpreadsheetPos(c.m_mousePos);
        auto str = DataStorage::get().getRawData(Pos(col, row));

        auto ctxRect = c.m_hoveredCell;
        c.r_layersRequest.get() = DialogCreateRequest{
                    Widget{std::move(ctxRect), w.getFillColor()}
                    , DialogCreateRequest::Payload{std::nullopt, std::move(str), MAX_CELL_TEXT_LENGTH}};
    } 
    return std::nullopt;
}

OptState IdleTableState::process(const MouseRightUpEvent& event) {
    auto& c = m_context.get();
    const auto& w = c.r_widget.get(); 
    c.m_mousePos = { event.x, event.y };

    // TODO to make it generic and let it support indices, there must be an array of hitboxes. 
    // The problem is that hitbox order is fixed. So... does it make sense to make this layer generic? 
    // -> maybe does, but it requires some constexpr stuff
    // request insertion/deletion menu
    if (w.columnSpaceContains(c.m_mousePos) || w.rowSpaceContains(c.m_mousePos)) {
        auto ctxRect = c.m_hoveredCell;
        TableOperationsActionHandler::requestMainMenu(HandlerContext{
            Widget{std::move(ctxRect), w.getFillColor(), w.getOutlineColor(), w.getCharWidth(), w.getCharHeight()}
            , c.r_layersRequest, c.getOperationView() });    
    } // else if (w.spreadsheetSpaceContains(c.m_mousePos)) { cell menu } 
    return std::nullopt;
}

OptState IdleTableState::process(const MouseMotionEvent& event) {
    auto& c = m_context.get();
    // update the hovered-on cell selection rectangle
    c.m_mousePos = { event.x, event.y };
    c.m_hoveredCell = c.r_widget.get().selectCell(c.m_mousePos);
    return std::nullopt;
}

OptState IdleTableState::process(const MouseScrollingEvent& event) {
    auto& c = m_context.get();
    auto& w = c.r_widget.get();
    
    // add scrolling offset and update the gridlines
    w.addOffset(event.x, event.y);
    w.update();
    c.m_hoveredCell = w.selectCell(c.m_mousePos);
    return std::nullopt;
}

// commits resizing
OptState ColumnResizingTableState::process(const MouseLeftUpEvent& event) {
    auto& c = m_context.get();
    c.r_widget.get().resizeColumnLabel(c.m_mousePos.x, event.x - c.m_mousePos.x);      
    c.m_mousePos = {event.x, event.y};  
    c.m_hoveredCell = c.r_widget.get().selectCell(c.m_mousePos);
    return IdleTableState{c}; 
}

// draws a special helper line during resizing
void ColumnResizingTableState::draw(SDL_Renderer* const renderer) const {
    auto hbox = m_context.get().r_widget.get().getHitBox();
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    float x;
    SDL_GetMouseState(&x, nullptr);
    SDL_FPoint helperLine[2] = {{x, hbox.y}, {x, hbox.y + hbox.h}};
    SDL_RenderLines(renderer, helperLine, 2);
}

// commits resizing
OptState RowResizingTableState::process(const MouseLeftUpEvent& event) {
    auto& c = m_context.get();
    auto& w = c.r_widget.get();
    w.resizeRowLabel(c.m_mousePos.y, event.y - c.m_mousePos.y);
    c.m_mousePos = {event.x, event.y}; 
    c.m_hoveredCell = w.selectCell(c.m_mousePos);
    return IdleTableState{c}; 
}

// draws a special helper line during resizing
void RowResizingTableState::draw(SDL_Renderer* const renderer) const {
    auto hbox = m_context.get().r_widget.get().getHitBox();
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    float y;
    SDL_GetMouseState(nullptr, &y); 
    SDL_FPoint helperLine[2] = {{hbox.x, y}, {hbox.x + hbox.w, y}};
    SDL_RenderLines(renderer, helperLine, 2);
}
