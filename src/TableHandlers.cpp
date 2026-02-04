#include "TableHandlers.h"
#include "Handlers.h"

bool TableOperationsActionHandler::dispatch(MenuResponse&& resp, HandlerContext ctx) {
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

bool TableCellActionHandler::dispatch(DialogResponse&& resp, HandlerContext ctx) {
    if (!ctx.op) return false;
    ctx.op->get() = WriteOperation{resp.text}; 
    return true; // TODO is it ok?
}
