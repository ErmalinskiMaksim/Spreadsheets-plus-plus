#include "TableInteractor.h"
// TableInteractor::TableInteractor(TableWidgetRef table, RequestRef req)
//     : m_hoveredCell{0.0f, 0.0f, 0.0f, 0.0f}
//     , m_mousePos{0.0f, 0.0f}
//     , r_widget(table)
//     , r_layersRequest(req)
//     , m_fsm{IdleTableState{ std::ref(*this)}}
// {}
//
// void TableInteractor::performInsert(bool isAfter) {
//     auto& w = r_widget.get(); 
//     auto& ds = DataStorage::get();
//
//     if (w.columnSpaceContains(m_mousePos))
//         ds.insertColumn(w.insertColumn(m_mousePos.x, isAfter));
//     else if (w.rowSpaceContains(m_mousePos))
//         ds.insertRow(w.insertRow(m_mousePos.y, isAfter));
//
//     m_hoveredCell = w.selectCell(m_mousePos);
// }

// void TableInteractor::performDelete() {
//     auto& w = r_widget.get(); 
//     auto& ds = DataStorage::get();
//
//     if (w.columnSpaceContains(m_mousePos))
//         ds.deleteColumn(w.deleteColumn(m_mousePos.x));
//     else if (w.rowSpaceContains(m_mousePos))
//         ds.deleteRow(w.deleteRow(m_mousePos.y));
//
//     m_hoveredCell = w.selectCell(m_mousePos);
// }

// void TableInteractor::performWrite(std::string&& cellInput) {
//     auto& w = r_widget.get();
//     auto pos = w.getSpreadsheetPos(m_mousePos);
//     w.shiftColumns(pos.col);
//     w.shiftRows(pos.row);
//     DataStorage::get().setData(std::move(cellInput), pos);
// }

// void TableInteractor::render(SDL_Renderer* renderer, const TextRenderer& txtRenderer) const {
//     // hovered-over rectangle
//     SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
//     SDL_RenderRect(renderer, &m_hoveredCell);
//
//     // state-dependent items like helper lines
//     m_fsm.draw(renderer);
//
//     // draw spreadsheet text content
//     const auto& storage = DataStorage::get().getRawStorage();
//     for(auto&& [blockPos, block] : storage)
//         for(auto pos = 0uz; pos < block.m_data.size(); ++pos) {
//             Pos tablePos = DataStorage::getCellAbsolutePos(blockPos, pos);
//             auto dest = r_widget.get().getSpreadsheetCoordinates(tablePos);
//             txtRenderer.render(renderer, dest, block.m_data[pos].text.c_str(), block.m_data[pos].text.length());
//         }
// }
