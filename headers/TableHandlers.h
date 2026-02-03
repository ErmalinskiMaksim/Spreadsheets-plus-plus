#ifndef TABLE_HANDLERS_H
#define TABLE_HANDLERS_H

#include "Handlers.h"

// an action handler for spreadsheet menus
class TableOperationsActionHandler {
    // actions of the column/row  
    enum class Actions : ActionID { INSERT_AFTER = 0x20, INSERT_BEFORE = 0x21, DELETE = 0x22 };
public:
    static constexpr std::string getID() { return "Table ops"; }

    // defines the outline of the column/row insertion.deletion menu
    // leaves a request to create a menu layer
    static bool requestMainMenu(HandlerContext ctx) {
        using MenuAction = MenuCreateRequest::Payload::MenuAction;
        auto& req = ctx.req.get();

        req = MenuCreateRequest {
                Widget {
                    SDL_FRect{ctx.widget.getHitBox().x, ctx.widget.getHitBox().y
                        , ctx.widget.getCharWidth() * 13, ctx.widget.getCharHeight() * 3}
                    , ctx.widget.getFillColor() - 0x22
                }
                , MenuCreateRequest::Payload {{ 
                    MenuAction{"Insert After", static_cast<ActionID>(Actions::INSERT_AFTER)} 
                  , MenuAction{"Insert Before", static_cast<ActionID>(Actions::INSERT_BEFORE)}
                  , MenuAction{"Delete", static_cast<ActionID>(Actions::DELETE)}
                }}};
        
        return true;
    }

    bool dispatch(MenuResponse&&, HandlerContext);
};

class TableCellActionHandler {
    // TODO cell menu
public:
    static constexpr std::string getID() { return "Table cell ops"; }
    static bool requestMainMenu(HandlerContext) { return true; }
    // TODO void requestMainMenu(HandlerContext);
    bool dispatch(DialogResponse&&, HandlerContext);
};

#endif
