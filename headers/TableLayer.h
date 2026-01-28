#ifndef TABLE_LAYER_H
#define TABLE_LAYER_H
#include "Layer.h"
#include "TableInteractor.h"
#include "TableWidget.h"

// The layer that is used in conjunction with a spreadsheet
class TableLayer final : public Layer {
public:
    TableLayer(Widget&&);
    /// Layeer's interface /// 
    void dispatchEvents(const GuiEvent&) override;
    void draw(SDL_Renderer* const, const TextRenderer&) const override;
    void onResponse(Responses&&) override;
    bool hitTest(float, float) const noexcept override;
protected:
    TableWidget m_tableWidget;
    TableInteractor m_interactor;
};
#endif 
