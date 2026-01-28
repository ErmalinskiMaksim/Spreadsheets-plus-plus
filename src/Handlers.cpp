#include "Handlers.h"
#include "FileManager.h"

// Recap.
// request a menu when first called
bool FileActionHandler::requestMainMenu(HandlerContext ctx) {
    using MenuAction = MenuCreateRequest::MenuAction;

    auto& req = ctx.req.get();
    const auto& widget = ctx.widget.get();
    auto hbox = widget.getHitBox();

    req = MenuCreateRequest{
            Widget {
                 SDL_FRect{hbox.x, hbox.y + hbox.h, widget.getCharWidth() * 5, widget.getCharHeight() * 3}
                , widget.getColor() 
            }
            , { MenuAction{"Open", static_cast<ActionID>(Actions::OPEN) }
              , MenuAction{"Save", static_cast<ActionID>(Actions::SAVE) }
              , MenuAction{"Close", static_cast<ActionID>(Actions::CLOSE)}
            }
        }; 
    return true;
}

// Recap.
// Recieve response from the menu:
// OPEN/SAVE ---> request a dialog
// CLOSE     ---> close the current file
// none      ---> do nothing
bool FileActionHandler::dispatch(MenuResponse&& resp, HandlerContext ctx) {
    auto& req = ctx.req.get();
    const auto& w = ctx.widget.get();
    auto hbox = w.getHitBox();
    auto color = w.getColor();
    auto charWidth = w.getCharWidth();
    auto charHeight = w.getCharHeight();

    m_currentAction = static_cast<Actions>(resp.code);
    switch (m_currentAction) {
        case Actions::OPEN:
            [[fallthrough]];
        case Actions::SAVE:
            req =  
                DialogCreateRequest{ 
                    Widget{ SDL_FRect{hbox.x, hbox.y + hbox.h, hbox.w, charHeight * 2}
                        , color - 0x11 
                        , charWidth
                        , charHeight
                    } 
                    ,"Enter the file path:"
                    , std::nullopt
                    , FileManager::MAX_FILE_PATH_LEN 
                };
            break;
        case Actions::CLOSE:
            DataStorage::get().clear();
            break;
        default: 
            return false;
    }
    return true;
}

// Recap:
// Get file path from the dialog
// * OPEN ---> load file
// * SAVE ---> save to file 
// * others -> do nothing
bool FileActionHandler::dispatch(DialogResponse&& resp, HandlerContext) {
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

// Recap.
// request a menu when first called
bool HelpActionHandler::requestMainMenu(HandlerContext ctx) {
    using MenuAction = MenuCreateRequest::MenuAction;

    auto& req = ctx.req.get();
    const auto& widget = ctx.widget.get();
    auto hbox = widget.getHitBox();

    req = MenuCreateRequest{
            Widget {
                SDL_FRect{hbox.x, hbox.y + hbox.h, widget.getCharWidth() * 10, widget.getCharHeight() }
              , widget.getColor()
            }
            , { MenuAction{"View Guide", static_cast<ActionID>(Actions::VIEW_GUIDE)}}
        }; 
    return true;
}

// Recap.
// Recieves response from the menu:
// * VIEW_GUIDE ---> requests a pop-up with information
// * others     ---> do nothing
bool HelpActionHandler::dispatch(MenuResponse&& resp, HandlerContext ctx) {
    auto& req = ctx.req.get();
    const auto& widget = ctx.widget.get();
    auto hbox = widget.getHitBox();

    switch (static_cast<Actions>(resp.code)) {
        case Actions::VIEW_GUIDE:
            req = PopupCreateRequest{
                Widget {
                    SDL_FRect{hbox.x, hbox.y + hbox.h, widget.getCharWidth()*34, widget.getCharHeight()*5}
                    , widget.getColor() - 0x11
                }
                , { "Write to a cell:     LMB click"
                  , "Commit text input:   Enter"
                  , "Resize a row/column: hold LMB+CTRL"
                  , "Exit application:    CTRL-Q"
                  , "Exit this pop-up:    LMB click"
                }
            };
            break;
        default: 
            return false;
    }
    return true;
}
