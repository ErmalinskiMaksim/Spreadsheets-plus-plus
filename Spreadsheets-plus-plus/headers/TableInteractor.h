#ifndef TABLE_INTERACTOR_H
#define TABLE_INTERACTOR_H

#include "Interactors/Interactor.h"
#include "TableStateMachine.h"
#include "DataStorage.h"

// A layer of indirection between layer event processing and the table widget
template<WidgetType MainWidget = TableWidget
        , ResponseHandler TableOps = TableOperationsActionHandler
        , ResponseHandler CellOps = TableCellActionHandler>
class TableInteractor : public Interactor {
    friend class IdleTableState;
    friend class ColumnResizingTableState;
    friend class RowResizingTableState;
    
    using TableWidgetRef = std::reference_wrapper<MainWidget>;
    using FSM = TableStateMachine<IdleTableState, ColumnResizingTableState, RowResizingTableState>; 
public:
    static constexpr bool hasOperations = true;

    TableInteractor(NonModalLayerCreateRequest::Payload&&, TableWidgetRef table, RequestView req)
    : m_operation{EmptyOperation{}} 
    , m_hoveredCell{0.0f, 0.0f, 0.0f, 0.0f}
    , m_mousePos{0.0f, 0.0f}
    , r_widget(table)
    , r_layersRequest(req)
    , m_fsm{IdleTableState{ std::ref(*this)}}
    {}

    void dispatchEvents(const LayerEvent& event) {
        std::visit([&](auto&& ev) { m_fsm.process(ev); }, event);
    }
    
    OperationView getOperation() {
        return std::ref(m_operation);
    } 

    void processOperation() {
        std::visit([&](auto&& op) { 
            using T = std::decay_t<decltype(op)>;
            if constexpr (std::is_same_v<T, InsertOperation>
                        || std::is_same_v<T, DeleteOperation>
                        || std::is_same_v<T, WriteOperation>) {
                perform(op); 
                m_operation = EmptyOperation{};
            } 
        }, m_operation);
    }

    void render(const Renderer& renderer, const Font& font) const {
        renderer.renderRect(&m_hoveredCell, {0x00, 0x00, 0xFF, 0xFF});

        m_fsm.draw(renderer, font);

        // draw spreadsheet text content
        const auto& storage = DataStorage::get().getRawStorage();
        for(auto&& [blockPos, block] : storage)
            for(auto pos = 0uz; pos < block.m_data.size(); ++pos) {
                Pos tablePos = DataStorage::getCellAbsolutePos(blockPos, pos);
                auto dest = r_widget.get().getSpreadsheetCoordinates(tablePos);
                renderer.renderText(font, dest, block.m_data[pos].text);
            }
    }

private:
    void perform(InsertOperation op) {
        auto& w = r_widget.get(); 
        auto& ds = DataStorage::get();

        if (w.columnSpaceContains(m_mousePos))
            ds.insertColumn(w.insertColumn(m_mousePos.x, op.isAfter));
        else if (w.rowSpaceContains(m_mousePos))
            ds.insertRow(w.insertRow(m_mousePos.y, op.isAfter));

        m_hoveredCell = w.selectCell(m_mousePos);
        
    }

    void perform(DeleteOperation) {
        auto& w = r_widget.get(); 
        auto& ds = DataStorage::get();

        if (w.columnSpaceContains(m_mousePos))
            ds.deleteColumn(w.deleteColumn(m_mousePos.x));
        else if (w.rowSpaceContains(m_mousePos))
            ds.deleteRow(w.deleteRow(m_mousePos.y));

        m_hoveredCell = w.selectCell(m_mousePos);
    }

    void perform(WriteOperation op) {
        auto& w = r_widget.get();
        auto pos = w.getSpreadsheetPos(m_mousePos);
        w.shiftColumns(pos.col);
        w.shiftRows(pos.row);
        DataStorage::get().setData(std::move(op.text), pos);
    }

    OperationRegister m_operation;
    // selection rectangle of the hovered-on cell
    Rect m_hoveredCell;
    // cached mouse position
    Point m_mousePos;
    // a reference to the table widget
    TableWidgetRef r_widget;
    // a reference to the layer's request slot
    RequestView r_layersRequest;
    // state machine
    FSM m_fsm;
};

#endif
