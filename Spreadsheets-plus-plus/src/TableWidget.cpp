#include "TableWidget.h"
#include <cstring>
#include <charconv>
#include "Pos.h"

TableWidget::TableWidget(Widget&& widget, size_t cols, size_t rows) 
    : Widget(widget)
    , m_cols{ Labels(cols, {m_hitBox.w/static_cast<float>(cols), {'\0', '\0', '\0', '\0'}})
            , Points(cols*2+3)
            , Rect{m_hitBox.x + m_hitBox.w*0.05f, m_hitBox.y, m_hitBox.w*0.95f, m_hitBox.h*0.05f}
            , m_hitBox.w/static_cast<float>(cols)
            , 0.0f 
            , m_hitBox.x + m_hitBox.w*0.05f
            , m_hitBox.w}
    , m_rows{ Labels(rows, {m_hitBox.h/static_cast<float>(rows), {'\0', '\0', '\0', '\0'}})
            , Points(rows*2+3)
            , Rect{m_hitBox.x, m_hitBox.y + m_hitBox.h*0.05f, m_hitBox.w*0.05f, m_hitBox.h*0.95f}
            , m_hitBox.h/static_cast<float>(rows)
            , 0.0f
            , m_hitBox.y + m_hitBox.h*0.05f
            , m_hitBox.h}
    , m_spreadsheetHitBox{
        m_cols.labelSpaceHitBox.x, m_rows.labelSpaceHitBox.y, m_cols.labelSpaceHitBox.w, m_rows.labelSpaceHitBox.h} 
{
        // initialize the existing labels 
        // column labels
        char label = 'A';
        for(auto&& a : m_cols.labels)
            a.label[0] = label++;
        // row labels
        for(auto i = 1uz; i < 10 && i <= rows; ++i) 
            m_rows.labels[i - 1].label[0] = static_cast<char>(i % 10) + '0';
        for(auto i = 10uz; i <= rows; ++i) { 
            m_rows.labels[i - 1].label[0] = static_cast<char>(i / 10) + '0';
            m_rows.labels[i - 1].label[1] = static_cast<char>(i % 10) + '0';
        }

        // initialize the grid points
        // vertical grid lines
        m_cols.gridLinePoints[0] = {m_hitBox.x, m_hitBox.y};
        m_cols.gridLinePoints[1] = {m_hitBox.x, m_hitBox.y + m_hitBox.h};
        m_cols.gridLinePoints[2] = {m_spreadsheetHitBox.x, m_hitBox.y + m_hitBox.h};   
        m_cols.gridLinePoints[3] = {m_spreadsheetHitBox.x, m_hitBox.y};
        updateColumnGridLines();

        // horizontal grid lines
        m_rows.gridLinePoints[0] = {m_hitBox.x, m_hitBox.y};
        m_rows.gridLinePoints[1] = {m_hitBox.x + m_hitBox.w, m_hitBox.y};
        m_rows.gridLinePoints[2] = {m_hitBox.x + m_hitBox.w, m_spreadsheetHitBox.y};
        m_rows.gridLinePoints[3] = {m_hitBox.x, m_spreadsheetHitBox.y};
        updateRowGridLines();
}

Pos TableWidget::getSpreadsheetPos(Point pos) const {
    auto col = m_cols.getLabelFromPos(pos.x).first;
    auto row = m_rows.getLabelFromPos(pos.y).first;
    return {col, row};
}

Rect TableWidget::getSpreadsheetCoordinates(const Pos& pos) const {
    auto [x, w] = m_cols.findStartOfLabel(pos.col);
    auto [y, h] = m_rows.findStartOfLabel(pos.row);
    return Rect(x, y, w, h);
}

Rect TableWidget::selectCell(Point pos) const {
    auto [x, w] = m_cols.findSelectedLabel(pos.x);
    auto [y, h] = m_rows.findSelectedLabel(pos.y);
    return Rect{x, y, w, h};
}

void TableWidget::addOffset(float dx, float dy) noexcept {
    // offset is added only to the prevailing dimension
    if (std::abs(dx) >= std::abs(dy)) m_cols.offset += dx * 6.0f;
    else m_rows.offset += dy * 6.0f;

    // offset can't get behind the spreadsheet space
    if (m_cols.offset < 0.0f) m_cols.offset = 0.0f; 
    if (m_rows.offset < 0.0f) m_rows.offset = 0.0f; 
}

size_t TableWidget::insertColumn(float x, bool insertAfter) {
    // find column's index
    auto idx = m_cols.getLabelFromPos(x).first;

    // if column is virtual -> materialize and shift
    if (idx >= m_cols.labels.size())
        shiftColumns(idx);

    // new label
    auto newLabel = m_cols.labels[idx];
    newLabel.dimension = m_cols.defaultDimension;

    if (insertAfter) ++idx;

    m_cols.labels.insert(m_cols.labels.begin() + static_cast<ptrdiff_t>(idx), newLabel);
    shiftColumns(idx);
    updateColumnGridLines();

    return idx;
}

size_t TableWidget::insertRow(float y, bool insertAfter) {
    // find row's index
    auto idx = m_rows.getLabelFromPos(y).first;

    // if row is virtual -> materialize and shift
    if (idx >= m_rows.labels.size())
        shiftRows(idx);

    // new label
    auto newLabel = m_rows.labels[idx];
    newLabel.dimension = m_rows.defaultDimension;

    if (insertAfter) ++idx;

    m_rows.labels.insert(m_rows.labels.begin() + static_cast<ptrdiff_t>(idx), newLabel);
    shiftRows(idx);
    updateRowGridLines();

    return idx;
}

size_t TableWidget::deleteColumn(float x) {
    // find column's index
    auto idx = m_cols.getLabelFromPos(x).first;
    
    // if the column is virtual, do nothing
    if (idx > m_cols.labels.size()) return idx;

    m_cols.labels.erase(m_cols.labels.begin() + static_cast<ptrdiff_t>(idx));
    shiftColumns(idx - 1);
    updateColumnGridLines();

    return idx;
}

size_t TableWidget::deleteRow(float y) {
    // find row's index
    auto idx = m_rows.getLabelFromPos(y).first;

    // if row is virtual, do nothing
    if (idx > m_rows.labels.size()) return idx;

    m_rows.labels.erase(m_rows.labels.begin() + static_cast<ptrdiff_t>(idx));
    shiftRows(idx - 1);
    updateRowGridLines();

    return idx;
}

void TableWidget::update() {
    updateColumnGridLines();
    updateRowGridLines();
}

void TableWidget::shiftColumns(size_t begin) {
    // materialize virtual labels
    auto oldSz = m_cols.labels.size();
    if (oldSz <= begin) {
        auto newSz = std::min(begin + 1, 17576uz);
        m_cols.labels.resize(newSz);
        // set default dimensions
        for (auto i = oldSz; i < newSz; ++i) 
            m_cols.labels[i].dimension = m_cols.defaultDimension;
        begin = oldSz;
    }

    // fill labels
    for (auto i = begin; i < m_cols.labels.size(); ++i) {
        int idx = static_cast<int>(i + 1);
        char tmp[4];
        int len = 0;

        while ((idx--) > 0 && len < 3) {
            tmp[len++] = static_cast<char>('A' + (idx % 26));
            idx /= 26;
        }   

        for (auto j = 0; j < len; ++j)
            m_cols.labels[i].label[j] = tmp[len - 1 - j];
        m_cols.labels[i].label[len] = '\0';
    }
}

void TableWidget::shiftRows(size_t begin) {
    // materialize virtual labels
    auto oldSz = m_rows.labels.size();
    if (oldSz <= begin) {
        auto newSz = std::min(begin + 1, 999uz);
        m_rows.labels.resize(newSz);
        // set default dimensions 
        for (auto i = oldSz; i < newSz; ++i)
            m_rows.labels[i].dimension = m_rows.defaultDimension;
        begin = oldSz;
    }

    // fill labels
    for (auto i = begin; i < m_rows.labels.size(); ++i) {
        auto [ptr, ec] = std::to_chars(m_rows.labels[i].label, m_rows.labels[i].label + 4, i + 1);
        if (ec == std::errc{}) *ptr = '\0';
    }
}

void TableWidget::resizeColumnLabel(float cellPos, float delta) {
    auto idx = m_cols.getLabelFromPos(cellPos).first;
    // if a virtual label is resized
    if (idx >= m_cols.labels.size()) shiftColumns(idx);
    m_cols.labels[idx].dimension = std::max(m_cols.labels[idx].dimension + delta
                                                , m_cols.labels[idx].dimension / 2);
    updateColumnGridLines();       
}

void TableWidget::resizeRowLabel(float cellPos, float delta) {
    auto idx = m_rows.getLabelFromPos(cellPos).first;
    // if a virtual label is resized
    if (idx >= m_rows.labels.size()) shiftRows(idx);
    m_rows.labels[idx].dimension = std::max(m_rows.labels[idx].dimension + delta
                                            , m_rows.labels[idx].dimension / 2);
    updateRowGridLines();   
}

void TableWidget::render(const Renderer& renderer, const Font& font) const {
    auto dest = m_cols.labelSpaceHitBox;
    m_cols.render(renderer, font, m_fillColor, dest, dest.x, dest.w);
    dest = m_rows.labelSpaceHitBox;
    m_rows.render(renderer, font, m_fillColor, dest, dest.y, dest.h);
}

bool TableWidget::columnSpaceContains(Point pos) const noexcept {
    return Widget::contains(pos.x, pos.y, m_cols.labelSpaceHitBox);
}

bool TableWidget::rowSpaceContains(Point pos) const noexcept {
    return Widget::contains(pos.x, pos.y, m_rows.labelSpaceHitBox);
}

bool TableWidget::spreadsheetSpaceContains(Point pos) const noexcept {
    return Widget::contains(pos.x, pos.y, m_spreadsheetHitBox);
}

void TableWidget::updateColumnGridLines() {
    auto [i, pos] = m_cols.findPosWithScrollingOffset(m_cols.logicalStart);

    // worst case scenario: defaultDimension == 1.0f
    auto maxLines = static_cast<size_t>(m_cols.logicalEnd / 1.0f) + 2;
    m_cols.gridLinePoints.resize(maxLines * 2 + 4);

    // set new grid points based on the labels
    auto j = 0uz;
    for (; pos < m_cols.logicalStart + m_cols.logicalEnd; ++j, ++i) {
        pos += (i < m_cols.labels.size()) ? m_cols.labels[i].dimension : m_cols.defaultDimension;
        bool odd = j % 2;
        m_cols.gridLinePoints[2*j+4] = { pos, m_hitBox.y + m_hitBox.h*odd}; 
        m_cols.gridLinePoints[2*j+1+4] = { pos, m_hitBox.y + m_hitBox.h*!odd};        
    }

    m_cols.gridLinePoints.resize(2*j + 4);
}

void TableWidget::updateRowGridLines() {
    auto [i, pos] = m_rows.findPosWithScrollingOffset(m_rows.logicalStart);

    // FIXME it's too much resizing. It should be improved
    // worst case scenario: defaultDimension == 1.0f
    auto maxLines = static_cast<size_t>(m_rows.logicalEnd / 1.0f) + 2;
    m_rows.gridLinePoints.resize(maxLines * 2 + 4);

    // set new grid points based on the labels
    auto j = 0uz;
    for (; pos < m_rows.logicalStart + m_rows.logicalEnd; ++j, ++i) {
        pos += (i < m_rows.labels.size()) ? m_rows.labels[i].dimension : m_rows.defaultDimension;
        bool odd = j % 2;
        m_rows.gridLinePoints[2*j+4] = {m_hitBox.x + m_hitBox.w*odd, pos }; 
        m_rows.gridLinePoints[2*j+1+4] = {m_hitBox.x + m_hitBox.w*!odd, pos };        
    }

    m_rows.gridLinePoints.resize(2*j + 4);
}

// returns a pair [index in the labels array; the coordinates of the label's start]
// if the position is outside the bounds of the label vector, then the return value
// is a virtual label that would have existed at pos index with a default dimension
std::pair<size_t, float> TableWidget::HalfTable::getLabelFromPos(float end) const {
    auto i = 0uz;
    float consumed = 0.0f;
    float logical = end + offset - logicalStart;

    while (i < labels.size() && logical >= consumed + labels[i].dimension)
        consumed += labels[i++].dimension;

    if (i >= labels.size()) {
        float remaining = logical - consumed;
        auto virtualLabels = static_cast<size_t>(remaining / defaultDimension);
        i += virtualLabels;
        consumed += static_cast<float>(virtualLabels) * defaultDimension;
    }

    float start = logicalStart+ consumed - offset;
    return { i, start };
}

std::pair<size_t, float> TableWidget::HalfTable::findPosWithScrollingOffset(float end) const {
    auto i = 0uz;
    float begin = offset;
    // real labels
    while (i < labels.size() && begin >= labels[i].dimension) begin -= labels[i++].dimension;
    // virtual labels
    if (i == labels.size()) {
        auto virtualLabels = static_cast<size_t>(begin / defaultDimension);
        begin -= static_cast<float>(virtualLabels) * defaultDimension;
        i += virtualLabels;
    }

    float cellStart = end - begin;
    return { i, cellStart };
}

Point TableWidget::HalfTable::findStartOfLabel(size_t idx) const {
    float pos = 0.0f;
    float dim = defaultDimension;

    // real labels
    if (idx < labels.size()) {
        auto i = 0uz;
        while (i < idx) pos += labels[i++].dimension;
        dim = labels[i].dimension;
    } else { // virtual labels
        for (auto&& label : labels) pos += label.dimension;
        pos += static_cast<float>(idx - labels.size()) * defaultDimension;
    }
    
    return { pos - offset + logicalStart, dim} ;
}

Point TableWidget::HalfTable::findSelectedLabel(float pos) const {
    Point selectedCell; // start and dimension
    // trim the coordinate to ignore everything before spreadsheet space
    auto [i, start] = getLabelFromPos(std::max(pos, logicalStart)); 
    selectedCell.y = (i < labels.size()) 
        ? labels[i].dimension : defaultDimension;
    if (start < logicalStart) {
        selectedCell.x = logicalStart;
        selectedCell.y -= (logicalStart - start);
    } else selectedCell.x = start;
    return selectedCell;
}

void TableWidget::HalfTable::render(const Renderer& renderer, const Font& font, 
        Color c, Rect& dest, float& begin, float& len) const {
    renderer.renderFillRect(&labelSpaceHitBox, c);

    // grid lines
    renderer.renderLines(gridLinePoints.data(), static_cast<int>(gridLinePoints.size()), c - 0x22);

    // labels
    auto [i, pos] = findPosWithScrollingOffset(logicalStart);
    auto sz = labels.size();
    pos -= logicalEnd;
    for(; i < sz && begin > pos; ++i) {
        len = labels[i].dimension;
        renderer.renderText(font, dest, std::string_view(labels[i].label, strnlen(labels[i].label, 3)));
        begin += len;
    }
}
