#ifndef TASK_BAR_HANDLERS_H
#define TASK_BAR_HANDLERS_H

#include "RTWgui/Requests.h"
// #include "HandlerContext.h"
#include "FileManager.h"

// Handles actions of the "File" menu
template<typename HandlerContext>
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
    consteval auto buildFilePathDialogPayload() {
        return DialogCreateRequest::Payload{
             "Enter the file path:"
            , std::nullopt
            , FileManager::MAX_FILE_PATH_LEN
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
                 Rect{hbox.x, hbox.y + hbox.h, ctx.widget.getCharWidth() * 5, ctx.widget.getCharHeight() * 3}
                , ctx.widget.getFillColor() 
            }
            , MenuCreateRequest::Payload{{std::begin(payload), std::end(payload)}}}; 

        return true;
    }

    // receive a chosen option from the menu
    // Recap.
    // Recieve response from the menu:
    // OPEN/SAVE ---> request a dialog
    // CLOSE     ---> close the current file
    // none      ---> do nothing
    bool dispatch(MenuResponse&& resp, HandlerContext ctx) {
        auto hbox = ctx.widget.getHitBox();
        auto color = ctx.widget.getFillColor();
        auto charWidth = ctx.widget.getCharWidth();
        auto charHeight = ctx.widget.getCharHeight();

        m_currentAction = static_cast<Actions>(resp.code);
        switch (m_currentAction) {
            case Actions::OPEN:
                [[fallthrough]];
            case Actions::SAVE:
                ctx.req.get() =  
                    DialogCreateRequest{ 
                        Widget{ Rect{hbox.x, hbox.y + hbox.h, hbox.w, charHeight * 2}
                            , color - 0x11 
                            , Color{}
                            , charWidth
                            , charHeight
                        }
                        , buildFilePathDialogPayload()};
                break;
            case Actions::CLOSE:
                DataStorage::get().clear();
                break;
            default: 
                return false;
        }
        return true;
    }

    // receive the file path from the dialog
    // Recap:
    // Get file path from the dialog
    // * OPEN ---> load file
    // * SAVE ---> save to file 
    // * others -> do nothing
    bool dispatch(DialogResponse&& resp, HandlerContext) {
        switch (m_currentAction) {
            case Actions::OPEN:
                FileManager::loadSpreadsheet(resp.text, Serializer{});
                break;
            case Actions::SAVE:
                FileManager::saveSpreadsheet(resp.text, Serializer{});
                break;
            case Actions::CLOSE:
                break;
            default:
                return false;
                break;
        }
        return true;
    }
    bool dispatch(PopupResponse&&, HandlerContext) { return false; }

private:
    Actions m_currentAction;
};

// Handles actions of the "Help" menu
template<typename HandlerContext>
class HelpActionHandler {
    // actions of the Help menu
    enum class Actions : ActionID { VIEW_GUIDE = 0x10};

    static consteval auto buildPayload() {
        using MenuAction = MenuCreateRequest::Payload::MenuAction;
        return std::array<MenuAction, 1> {
            MenuAction{"View Guide", static_cast<ActionID>(Actions::VIEW_GUIDE)}
        };
    } 

    static consteval auto buildHelpPopUpPayload() {
        return std::array<std::string_view, 5> {
             "Write to a cell:     LMB click"
           , "Commit text input:   Enter"
           , "Resize a row/column: hold LMB+CTRL"
           , "Exit application:    CTRL-Q"
           , "Exit this pop-up:    LMB click"
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
                    Rect{hbox.x, hbox.y + hbox.h, ctx.widget.getCharWidth() * 10, ctx.widget.getCharHeight() }
                  , ctx.widget.getFillColor()
                }
                , MenuCreateRequest::Payload{{std::begin(payload), std::end(payload)}}}; 

        return true;
    }
    // receives the chosen action.
    // Recap.
    // Recieves response from the menu:
    // * VIEW_GUIDE ---> requests a pop-up with information
    // * others     ---> do nothing
    bool dispatch(MenuResponse&& resp, HandlerContext ctx) {
        constexpr auto payload = buildHelpPopUpPayload();

        auto hbox = ctx.widget.getHitBox();
        switch (static_cast<Actions>(resp.code)) {
            case Actions::VIEW_GUIDE:
                ctx.req.get() = PopupCreateRequest{
                    Widget {
                        Rect{hbox.x, hbox.y + hbox.h, ctx.widget.getCharWidth()*34, ctx.widget.getCharHeight()*5}
                        , ctx.widget.getFillColor() - 0x11
                    }
                    , PopupCreateRequest::Payload{{std::begin(payload), std::end(payload)}}};
                break;
            default: 
                return false;
        }
        return true;
    }

    bool dispatch(DialogResponse&&, HandlerContext) { return false; }
    bool dispatch(PopupResponse&&, HandlerContext) { return false; }
};

#endif
