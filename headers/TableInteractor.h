#ifndef TABLE_INTERACTOR_H
#define TABLE_INTERACTOR_H
#include "Events.h"
#include "Handlers.h"

// an action handler for spreadsheet menus. It is not yet 
// the same general handler as in Handlers.cpp file.
// It will be fixed in the future
class TableWidgetActionHandler {
    enum class Actions : ActionID { INSERT_AFTER, INSERT_BEFORE, DELETE };
public:
    void requestMainMenu(HandlerContext);
    // TODO bool dispatch(MenuResponse&&, HandlerContext);
    // TODO bool dispatch(DialogResponse&&, HandlerContext);
};

class TableInteractor;
class TableWidget;
class SDL_Renderer;
class TextRenderer;
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
    explicit BaseTableState(std::reference_wrapper<TableInteractor> context) 
        : m_context(context) {}
    // supported events
    template<GuiEventType Event>
    OptState process(const Event&);
protected:
    // Table interactor reference
    std::reference_wrapper<TableInteractor> m_context;
};

template<typename T>
concept TableStateType = std::is_base_of_v<BaseTableState, T>;

class IdleTableState : public BaseTableState {
public:
    explicit IdleTableState(std::reference_wrapper<TableInteractor> context)
        : BaseTableState(context) {}
    // supported events
    using BaseTableState::process;
    OptState process(const MouseLeftDownEvent&);
    OptState process(const MouseRightUpEvent&);
    OptState process(const MouseMotionEvent&);
    OptState process(const MouseScrollingEvent&);
    // supported rendering logic
    void draw(SDL_Renderer* const) const {}
};

class ColumnResizingTableState : public BaseTableState {
public:
    explicit ColumnResizingTableState(std::reference_wrapper<TableInteractor> context) 
        : BaseTableState(context) {}
    // supported events
    using BaseTableState::process;
    OptState process(const MouseLeftUpEvent&);
    // supported rendering logic
    void draw(SDL_Renderer* const) const;
};

class RowResizingTableState : public BaseTableState {
public:
    explicit RowResizingTableState(std::reference_wrapper<TableInteractor> context) 
        : BaseTableState(context) {}
    // supported events
    using BaseTableState::process;
    OptState process(const MouseLeftUpEvent&);
    // supported rendering logic
    void draw(SDL_Renderer* const) const;
};

// nothing by default
template<GuiEventType Event>
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
    template<GuiEventType Event>
    void process(const Event& event) {
        auto optResult = std::visit([&](auto& state) { return state.process(event); }, m_state);
        if (optResult) m_state = std::move(*optResult);
    }
    // dispatches states
    void draw(SDL_Renderer* const renderer) const {
        std::visit([&](auto& state) { return state.draw(renderer); }, m_state);
    }
private:
    std::variant<States...> m_state;
};

// A layer of indirection between layer event processing and the table widget
class TableInteractor {
    friend class IdleTableState;
    friend class ColumnResizingTableState;
    friend class RowResizingTableState;
    enum class Actions : ActionID { INSERT_AFTER, INSERT_BEFORE, DELETE };

    using TableWidgetRef = std::reference_wrapper<TableWidget>;
    using RequestRef = std::reference_wrapper<OptRequest>;
    using FSM = TableStateMachine<IdleTableState, ColumnResizingTableState, RowResizingTableState>; 
public:
    TableInteractor(TableWidgetRef, RequestRef);

    // supported events
    template<GuiEventType Event>
    void processEvents(const Event& event) {
        m_fsm.process(event);
    }

    // response dispatching
    void dispatchResponse(Responses&&);
    void render(SDL_Renderer*, const TextRenderer&) const;
private:
    // supported responses
    void dispatch(MenuResponse&&);
    void dispatch(DialogResponse&&);
    void dispatch(PopupResponse&&){}

    // selection rectangle of the hovered-on cell
    SDL_FRect m_hoveredCell;
    // cached mouse position
    SDL_FPoint m_mousePos;
    // a reference to the table widget
    TableWidgetRef m_widget;
    // a reference to the layer's request slot
    RequestRef m_layersRequest;
    // state machine
    FSM m_fsm;
    // menu action handler
    TableWidgetActionHandler m_handler;
};
#endif
