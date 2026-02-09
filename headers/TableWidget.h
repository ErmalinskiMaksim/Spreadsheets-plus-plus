#ifndef TABLE_WIDGET_H
#define TABLE_WIDGET_H

#include "Widget.h"
#include <vector>

class TextRenderer;
class Pos;

class TableWidget : public Widget {
    struct HalfTable;
    struct SpreadsheetLabel;
    using Labels = std::vector<SpreadsheetLabel>;
    using Points = std::vector<Point>;
public:
    TableWidget(Widget&&, size_t cols = 12, size_t rows = 20);
    
    // convert graphical coordinates to <col, row>
    Pos getSpreadsheetPos(Point) const;
    // convert <col, row> to cell rect 
    Rect getSpreadsheetCoordinates(const Pos&) const;

    // get cell bounding rect which contains a point 
    Rect selectCell(Point) const;
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

    void render(const Renderer&, const Font&) const;

    bool columnSpaceContains(Point) const noexcept;
    bool rowSpaceContains(Point) const noexcept;
    bool spreadsheetSpaceContains(Point) const noexcept;
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
        Point findStartOfLabel(size_t) const;
        // finds the first visible label 
        // returns its coordinate and dimension
        Point findSelectedLabel(float) const;
        void render(const Renderer&, const Font&, Color, Rect&, float&, float&) const;

        // column or row labels
        Labels labels;
        // vertical or horizontal gridlines
        Points gridLinePoints;
        // bounding rect of 
        Rect labelSpaceHitBox;
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
    Rect m_spreadsheetHitBox;
};

#endif
