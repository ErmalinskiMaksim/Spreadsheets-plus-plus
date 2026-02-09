#ifndef TABLE_STATE_MACHINE_H
#define TABLE_STATE_MACHINE_H

#include "TableHandlers.h"
#include "TableWidget.h"
#include "Events.h"

template<WidgetType, ResponseHandler, ResponseHandler>
class TableInteractor;
using DefaultTableInteractor = TableInteractor<TableWidget, TableOperationsActionHandler, TableCellActionHandler>;

class IdleTableState;
class ColumnResizingTableState;
class RowResizingTableState;

// state machine states
// Idle state = default state which processes 
// scrolling, mouse motion, clicks
// It may enter column/row resizing state. 
// When resizing is finished the state returns to idle
using States = std::variant<
              IdleTableState
            , ColumnResizingTableState 
            , RowResizingTableState
        >;
using OptState = std::optional<States>;

// Base state of the state machine. Does nothing by default
class BaseTableState {
public:
    explicit BaseTableState(std::reference_wrapper<DefaultTableInteractor> context) 
        : m_context(context) {}
    // supported events
    template<LayerEventType Event>
    OptState process(const Event&);
protected:
    // Table interactor reference
    std::reference_wrapper<DefaultTableInteractor> m_context;
};

template<typename T>
concept TableStateType = std::is_base_of_v<BaseTableState, T>;

class IdleTableState : public BaseTableState {
public:
    explicit IdleTableState(std::reference_wrapper<DefaultTableInteractor> context)
        : BaseTableState(context) {}
    // supported events
    using BaseTableState::process;
    OptState process(const MouseLeftDownEvent&);
    OptState process(const MouseRightUpEvent&);
    OptState process(const MouseMotionEvent&);
    OptState process(const MouseScrollingEvent&);
    // supported rendering logic
    void draw(const Renderer&, const Font&) const {}
};

class ColumnResizingTableState : public BaseTableState {
public:
    explicit ColumnResizingTableState(std::reference_wrapper<DefaultTableInteractor> context) 
        : BaseTableState(context) {}
    // supported events
    using BaseTableState::process;
    OptState process(const MouseLeftUpEvent&);
    // supported rendering logic
    void draw(const Renderer&, const Font&) const;
};

class RowResizingTableState : public BaseTableState {
public:
    explicit RowResizingTableState(std::reference_wrapper<DefaultTableInteractor> context) 
        : BaseTableState(context) {}
    // supported events
    using BaseTableState::process;
    OptState process(const MouseLeftUpEvent&);
    // supported rendering logic
    void draw(const Renderer&, const Font&) const;
};

// nothing by default
template<LayerEventType Event>
OptState BaseTableState::process(const Event&) {
    return OptState{};
}

// state machine 
template<TableStateType... States>
class TableStateMachine {
public:
    template<TableStateType InitialState>
    explicit TableStateMachine(InitialState&& state) 
        : m_state(std::forward<InitialState>(state)) {}
    // dispatches states 
    template<LayerEventType Event>
    void process(const Event& event) {
        auto optResult = std::visit([&](auto& state) { return state.process(event); }, m_state);
        if (optResult) m_state = std::move(*optResult);
    }
    // dispatches states
    void draw(const Renderer& renderer, const Font& font) const {
        std::visit([&](auto&& state) { return state.draw(renderer, font); }, m_state);
    }
private:
    std::variant<States...> m_state;
};

#endif
