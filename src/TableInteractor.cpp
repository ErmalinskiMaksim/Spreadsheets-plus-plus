#include "TableInteractor.h"
#include "TableWidget.h"
#include "DataStorage.h"
#include "TextRenderer.h"

// defines the outline of the column/row insertion.deletion menu
// leaves a request to create a menu layer
void TableWidgetActionHandler::requestMainMenu(HandlerContext ctx) {
    using MenuAction = MenuCreateRequest::MenuAction;
    
    auto& req = ctx.req.get();
    const auto& widget = ctx.widget.get();

    req = MenuCreateRequest {
                Widget {
                    SDL_FRect{widget.getHitBox().x, widget.getHitBox().y
                        , widget.getCharWidth() * 13, widget.getCharHeight() * 3}
                    , widget.getColor() - 0x22
                }
                , { MenuAction{"Insert After", static_cast<ActionID>(Actions::INSERT_AFTER)} 
                  , MenuAction{"Insert Before", static_cast<ActionID>(Actions::INSERT_BEFORE)}
                  , MenuAction{"Delete", static_cast<ActionID>(Actions::DELETE)}
                }};

}

OptState IdleTableState::process(const MouseLeftDownEvent& event) {
    auto& c = m_context.get();
    const auto& w = c.m_widget.get();

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
        c.m_layersRequest.get() = DialogCreateRequest{
                    Widget{std::move(ctxRect), w.getColor()}
                    , std::nullopt, std::move(str), MAX_CELL_TEXT_LENGTH};
    } 
    return std::nullopt;
}

OptState IdleTableState::process(const MouseRightUpEvent& event) {
    auto& c = m_context.get();
    const auto& w = c.m_widget.get(); 
    c.m_mousePos = { event.x, event.y };
    // request insertion/deletion menu
    if (w.columnSpaceContains(c.m_mousePos) || w.rowSpaceContains(c.m_mousePos)) {
        auto ctxRect = c.m_hoveredCell;
        auto widgetCtx = Widget{ std::move(ctxRect), w.getColor(), w.getCharWidth(), w.getCharHeight()};
        c.m_handler.requestMainMenu(HandlerContext{c.m_layersRequest, widgetCtx});    
    } 
    return std::nullopt;
}

OptState IdleTableState::process(const MouseMotionEvent& event) {
    auto& c = m_context.get();
    // update the hovered-on cell selection rectangle
    c.m_mousePos = { event.x, event.y };
    c.m_hoveredCell = c.m_widget.get().selectCell(c.m_mousePos);
    return std::nullopt;
}

OptState IdleTableState::process(const MouseScrollingEvent& event) {
    auto& c = m_context.get();
    auto& w = c.m_widget.get();
    
    // add scrolling offset and update the gridlines
    w.addOffset(event.x, event.y);
    w.update();
    c.m_hoveredCell = w.selectCell(c.m_mousePos);
    return std::nullopt;
}

// commits resizing
OptState ColumnResizingTableState::process(const MouseLeftUpEvent& event) {
    auto& c = m_context.get();
    c.m_widget.get().resizeColumnLabel(c.m_mousePos.x, event.x - c.m_mousePos.x);      
    c.m_mousePos = {event.x, event.y};  
    c.m_hoveredCell = c.m_widget.get().selectCell(c.m_mousePos);
    return IdleTableState{c}; 
}

// draws a special helper line during resizing
void ColumnResizingTableState::draw(SDL_Renderer* const renderer) const {
    auto hbox = m_context.get().m_widget.get().getHitBox();
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    float x;
    SDL_GetMouseState(&x, nullptr);
    SDL_FPoint helperLine[2] = {{x, hbox.y}, {x, hbox.y + hbox.h}};
    SDL_RenderLines(renderer, helperLine, 2);
}

// commits resizing
OptState RowResizingTableState::process(const MouseLeftUpEvent& event) {
    auto& c = m_context.get();
    auto& w = c.m_widget.get();
    w.resizeRowLabel(c.m_mousePos.y, event.y - c.m_mousePos.y);
    c.m_mousePos = {event.x, event.y}; 
    c.m_hoveredCell = w.selectCell(c.m_mousePos);
    return IdleTableState{c}; 
}

// draws a special helper line during resizing
void RowResizingTableState::draw(SDL_Renderer* const renderer) const {
    auto hbox = m_context.get().m_widget.get().getHitBox();
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
    float y;
    SDL_GetMouseState(nullptr, &y); 
    SDL_FPoint helperLine[2] = {{hbox.x, y}, {hbox.x + hbox.w, y}};
    SDL_RenderLines(renderer, helperLine, 2);
}

TableInteractor::TableInteractor(TableWidgetRef table, RequestRef req)
    : m_hoveredCell{0.0f, 0.0f, 0.0f, 0.0f}
    , m_mousePos{0.0f, 0.0f}
    , m_widget(table)
    , m_layersRequest(req)
    , m_fsm{IdleTableState{ std::ref(*this)}}
    , m_handler{}
{}

void TableInteractor::dispatchResponse(Responses&& resp) {
    std::visit([&](auto&& r) { dispatch(std::forward<decltype(r)>(r)); }, std::forward<Responses>(resp));
} 

void TableInteractor::render(SDL_Renderer* renderer, const TextRenderer& txtRenderer) const {
    // hovered-over rectangle
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderRect(renderer, &m_hoveredCell);

    // state-dependent items like helper lines
    m_fsm.draw(renderer);

    // draw spreadsheet text content
    const auto& storage = DataStorage::get().getRawStorage();
    for(auto&& [blockPos, block] : storage)
        for(auto pos = 0uz; pos < block.m_data.size(); ++pos) {
            Pos tablePos = DataStorage::getCellAbsolutePos(blockPos, pos);
            auto dest = m_widget.get().getSpreadsheetCoordinates(tablePos);
            txtRenderer.render(renderer, dest, block.m_data[pos].text.c_str(), block.m_data[pos].text.length());
        }
}

// handle a menu Response 
// TODO move it to action handler
void TableInteractor::dispatch(MenuResponse&& resp) {
    auto& w = m_widget.get();
    if (w.columnSpaceContains(m_mousePos)) {
        auto& ds = DataStorage::get();
        switch(static_cast<Actions>(resp.code)) {
            case Actions::INSERT_AFTER:
                ds.insertColumn(w.insertColumn(m_mousePos.x, true));
                break;
            case Actions::INSERT_BEFORE:
                ds.insertColumn(w.insertColumn(m_mousePos.x));
                break;
            case Actions::DELETE:
                ds.deleteColumn(w.deleteColumn(m_mousePos.x));
                break;
            default:
                break;
        }
    } else if(w.rowSpaceContains(m_mousePos)) {
        auto& ds = DataStorage::get();
        switch(static_cast<Actions>(resp.code)) {
            case Actions::INSERT_AFTER:
                ds.insertRow(w.insertRow(m_mousePos.y, true));
                break;
            case Actions::INSERT_BEFORE:
                ds.insertRow(w.insertRow(m_mousePos.y));
                break;
            case Actions::DELETE:
                ds.deleteRow(w.deleteRow(m_mousePos.y));
                break;
            default:
                break;
        }
    }
    m_hoveredCell = w.selectCell(m_mousePos);
}

// handle dialog response 
// TODO move it to action handler
void TableInteractor::dispatch(DialogResponse&& resp) {
    auto& w = m_widget.get();
    auto pos = w.getSpreadsheetPos(m_mousePos);
    w.shiftColumns(pos.col);
    w.shiftRows(pos.row);
    DataStorage::get().setData(std::move(resp.text), pos);
}
