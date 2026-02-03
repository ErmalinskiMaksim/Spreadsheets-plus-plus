#include "TaskBarHandlers.h"
#include "FileManager.h"

// Recap.
// Recieve response from the menu:
// OPEN/SAVE ---> request a dialog
// CLOSE     ---> close the current file
// none      ---> do nothing
bool FileActionHandler::dispatch(MenuResponse&& resp, HandlerContext ctx) {
    auto& req = ctx.req.get();
    auto hbox = ctx.widget.getHitBox();
    auto color = ctx.widget.getFillColor();
    auto charWidth = ctx.widget.getCharWidth();
    auto charHeight = ctx.widget.getCharHeight();

    m_currentAction = static_cast<Actions>(resp.code);
    switch (m_currentAction) {
        case Actions::OPEN:
            [[fallthrough]];
        case Actions::SAVE:
            req =  
                DialogCreateRequest{ 
                    Widget{ SDL_FRect{hbox.x, hbox.y + hbox.h, hbox.w, charHeight * 2}
                        , color - 0x11 
                        , Color{}
                        , charWidth
                        , charHeight
                    }
                    , DialogCreateRequest::Payload{
                         "Enter the file path:"
                        , std::nullopt
                        , FileManager::MAX_FILE_PATH_LEN 
                    }};
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
// Recieves response from the menu:
// * VIEW_GUIDE ---> requests a pop-up with information
// * others     ---> do nothing
bool HelpActionHandler::dispatch(MenuResponse&& resp, HandlerContext ctx) {
    auto& req = ctx.req.get();
    auto hbox = ctx.widget.getHitBox();

    switch (static_cast<Actions>(resp.code)) {
        case Actions::VIEW_GUIDE:
            req = PopupCreateRequest{
                Widget {
                    SDL_FRect{hbox.x, hbox.y + hbox.h, ctx.widget.getCharWidth()*34, ctx.widget.getCharHeight()*5}
                    , ctx.widget.getFillColor() - 0x11
                }
                , PopupCreateRequest::Payload{{ 
                    "Write to a cell:     LMB click"
                  , "Commit text input:   Enter"
                  , "Resize a row/column: hold LMB+CTRL"
                  , "Exit application:    CTRL-Q"
                  , "Exit this pop-up:    LMB click"
                }}};
            break;
        default: 
            return false;
    }
    return true;
}
