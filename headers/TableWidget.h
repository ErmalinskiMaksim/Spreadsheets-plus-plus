#ifndef TABLE_WIDGET_H
#define TABLE_WIDGET_H

#include "Widget.h"
#include <SDL3/SDL_rect.h>
#include <vector>

class TextRenderer;
class Pos;

class TableWidget : public Widget {
    struct HalfTable;
    struct SpreadsheetLabel;
    using Labels = std::vector<SpreadsheetLabel>;
    using Points = std::vector<SDL_FPoint>;
public:
    TableWidget(Widget&&, size_t cols = 12, size_t rows = 20);
    
    // convert graphical coordinates to <col, row>
    Pos getSpreadsheetPos(SDL_FPoint) const;
    // convert <col, row> to cell rect 
    SDL_FRect getSpreadsheetCoordinates(const Pos&) const;

    // get cell bounding rect which contains a point 
    SDL_FRect selectCell(SDL_FPoint) const;
    // add <dx, dy> to offset 
    void addOffset(float, float) noexcept;

    // insert a column/row
    size_t insertColumn(float, bool insertAfter = false);
    size_t insertRow(float, bool insertAfter = false);

    // delete a column/row
    size_t deleteColumn(float);
    size_t deleteRow(float);

    // update grid lines
    void update();

    // shift columns/rows right starting from <index>
    void shiftColumns(size_t);
    void shiftRows(size_t);

    // resize a column/row label at <offset from start> by <delta>
    void resizeColumnLabel(float, float);
    void resizeRowLabel(float, float);

    void render(SDL_Renderer* const, const TextRenderer&) const;

    bool columnSpaceContains(SDL_FPoint) const noexcept;
    bool rowSpaceContains(SDL_FPoint) const noexcept;
    bool spreadsheetSpaceContains(SDL_FPoint) const noexcept;
private:
    // update column/row grid lines 
    void updateColumnGridLines();
    void updateRowGridLines();

    struct SpreadsheetLabel {
        float dimension; // the mutable dimension: either width or height depending on the context 
        char label[4]; // columns:[A-ZZZ] rows: [1-999], because label[3]='\0'
    };

    // this struct represents common data and operations 
    // assosiated with column and row components of the table
    struct HalfTable {  
        // get index and offset from the start of label space
        std::pair<size_t, float> getLabelFromPos(float) const;
        // finds the first visible coordinate (taking into account logical offset)
        // retuns a pair of an index of a cell which contains this coordinate and the coordinate itself 
        std::pair<size_t, float> findPosWithScrollingOffset(float) const;
        // find the offset from the start of label space of a label and dimension at a given index 
        SDL_FPoint findStartOfLabel(size_t) const;
        // finds the first visible label 
        // returns its coordinate and dimension
        SDL_FPoint findSelectedLabel(float) const;
        void render(SDL_Renderer* const, const TextRenderer&, Color, SDL_FRect&, float&, float&) const;

        // column or row labels
        Labels labels;
        // vertical or horizontal gridlines
        Points gridLinePoints;
        // bounding rect of 
        SDL_FRect labelSpaceHitBox;
        // a default value of SpreadsheetLabel::dimension
        const float defaultDimension;
        // scrolling offset
        float offset;
        // for columns logical start = labelSpaceHitBox.x
        // for rows = labelSpaceHitBox.y
        float logicalStart;
        float logicalEnd;
    };

    HalfTable m_cols;
    HalfTable m_rows;
    // a part of the hibox excluding column/row spaces
    SDL_FRect m_spreadsheetHitBox;
};

#endif
