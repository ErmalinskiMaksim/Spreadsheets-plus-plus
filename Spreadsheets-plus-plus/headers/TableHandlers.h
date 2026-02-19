#ifndef TABLE_HANDLERS_H
#define TABLE_HANDLERS_H

#include "RTWgui/Requests.h"
#include "Operations.h"

// an action handler for spreadsheet menus
template<typename HandlerContext>
class TableOperationsActionHandler {
    // actions of the column/row  
    enum class Actions : ActionID { INSERT_AFTER = 0x20, INSERT_BEFORE = 0x21, DELETE = 0x22 };

    static consteval auto buildPayload() {
        using MenuAction = MenuCreateRequest::Payload::MenuAction;
        return std::array<MenuAction, 3> {
              MenuAction{"Insert After", static_cast<ActionID>(Actions::INSERT_AFTER)} 
            , MenuAction{"Insert Before", static_cast<ActionID>(Actions::INSERT_BEFORE)}
            , MenuAction{"Delete", static_cast<ActionID>(Actions::DELETE)}
        };
    }

public:
    static constexpr std::string_view getID() { return "Table ops"; }

    // defines the outline of the column/row insertion.deletion menu
    // leaves a request to create a menu layer
    static bool requestMainMenu(HandlerContext ctx) {
        constexpr auto payload = buildPayload();

        const auto& w = ctx.widget.get();
        const auto& hbox = ctx.rect->get();
        ctx.req.get() = MenuCreateRequest {
            Widget {
                Rect{hbox.x, hbox.y, w.getCharWidth() * 13, w.getCharHeight() * 3}
                , {0xCC, 0xCC, 0xCC, 0xFF}
                , {0x00, 0x00, 0x00, 0xFF} 
            }
            , MenuCreateRequest::Payload {{ std::begin(payload), std::end(payload)}}};
        
        return true;
    }

    bool dispatch(MenuResponse&& resp, HandlerContext ctx) {
        if (!ctx.op) return false;
        switch (static_cast<Actions>(resp.code)) {
        case Actions::INSERT_AFTER:
            ctx.op->get() = InsertOperation{true};            
            break;
        case Actions::INSERT_BEFORE:
            ctx.op->get() = InsertOperation{false};            
            break;
        case Actions::DELETE:
            ctx.op->get() = DeleteOperation{};            
            break;
        default:
            ctx.op->get() = EmptyOperation{};
            return false;
        }
        return true;
    }
};

template<typename HandlerContext>
class TableCellActionHandler {
    // TODO cell menu
public:
    static constexpr std::string_view getID() { return "Table cell ops"; }
    static bool requestMainMenu(HandlerContext) { return true; }
    // TODO void requestMainMenu(HandlerContext);
    bool dispatch(DialogResponse&& resp, HandlerContext ctx) {
        if (!ctx.op) return false;
        ctx.op->get() = WriteOperation{std::move(resp.text)}; 
        return true; // TODO is it ok?
    }
};

#endif
