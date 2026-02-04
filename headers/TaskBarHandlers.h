#ifndef TASK_BAR_HANDLERS_H
#define TASK_BAR_HANDLERS_H

#include "Handlers.h"

// Handles actions of the "File" menu
class FileActionHandler {
    // actions of the "File" menu 
    enum class Actions : ActionID { OPEN = 0x01, SAVE = 0x02, CLOSE = 0x03 };
    
    static consteval auto buildPayload() {
        using MenuAction = MenuCreateRequest::Payload::MenuAction;
        return std::array<MenuAction, 3> {
                MenuAction{"Open", static_cast<ActionID>(Actions::OPEN) }
              , MenuAction{"Save", static_cast<ActionID>(Actions::SAVE) }
              , MenuAction{"Close", static_cast<ActionID>(Actions::CLOSE)}
        };
    }

public:
    static constexpr std::string_view getID() { return "File"; }

    // request a menu when first called
    // called when File menu is first invoked to display possible actions
    static bool requestMainMenu(HandlerContext ctx) {
        constexpr auto payload = buildPayload(); 

        auto hbox = ctx.widget.getHitBox();
        ctx.req.get() = MenuCreateRequest{
            Widget {
                 SDL_FRect{hbox.x, hbox.y + hbox.h, ctx.widget.getCharWidth() * 5, ctx.widget.getCharHeight() * 3}
                , ctx.widget.getFillColor() 
            }
            , MenuCreateRequest::Payload{{std::begin(payload), std::end(payload)}}}; 

        return true;
    }

    // receive a chosen option from the menu
    bool dispatch(MenuResponse&&, HandlerContext);
    // receive the file path from the dialog
    bool dispatch(DialogResponse&&, HandlerContext);
private:
    Actions m_currentAction;
};

// Handles actions of the "Help" menu
class HelpActionHandler {
    // actions of the Help menu
    enum class Actions : ActionID { VIEW_GUIDE = 0x10};

    static consteval auto buildPayload() {
        using MenuAction = MenuCreateRequest::Payload::MenuAction;
        return std::array<MenuAction, 1> {
            MenuAction{"View Guide", static_cast<ActionID>(Actions::VIEW_GUIDE)}
        };
    } 

public:
    static constexpr std::string_view getID() { return "Help"; }

    // request a menu when first called
    // called when Help menu is first invoked to display possible actions
    static bool requestMainMenu(HandlerContext ctx) {
        constexpr auto payload = buildPayload();

        auto hbox = ctx.widget.getHitBox();
        ctx.req.get() = MenuCreateRequest{
                Widget {
                    SDL_FRect{hbox.x, hbox.y + hbox.h, ctx.widget.getCharWidth() * 10, ctx.widget.getCharHeight() }
                  , ctx.widget.getFillColor()
                }
                , MenuCreateRequest::Payload{{std::begin(payload), std::end(payload)}}}; 

        return true;
    }
    // receives the chosen action.
    bool dispatch(MenuResponse&&, HandlerContext);
};

#endif
