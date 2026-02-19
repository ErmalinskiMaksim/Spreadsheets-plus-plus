#include "DataStorage.h"
#include <utility>
#include <algorithm>

// convert percents to string
std::string Percent::toString() const noexcept{ 
    // TODO add precision support like std::to_chars(buffer, end, value_, std::chars_format::fixed, 2);
    char buffer[64];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer) - 1, value);
    if (ec != std::errc{}) {
        // Should never happen but just in case
        return {};
    }

    *ptr++ = '%';
    return std::string(buffer, ptr);
}

// empty != non-existant. Empty = all cells have empty strings 
bool DataStorage::SpreadsheetBlock::empty() const noexcept {
    for (auto&& cell : m_data) 
        if (!cell.text.empty()) 
            return false; 
    return true;
}

bool DataStorage::contains(const Pos& pos) const noexcept {
    return m_blocks.contains(pos);
}

// returns FORMATED data
std::optional<std::reference_wrapper<const DataStorage::Format>>
DataStorage::getValue(const Pos& pos) const noexcept {
    auto cell = cellAtPos(pos);
    // check if has no format, return nothing
    if (!cell || std::holds_alternative<std::monostate>(cell->get().formattedView))
        return std::nullopt;

    return std::cref(cell->get().formattedView);
}

std::optional<std::string_view>
DataStorage::getRawData(const Pos& pos) const noexcept {
    auto cell = cellAtPos(pos);
    // if cell doesn't exist return nothing
    if(!cell) return std::nullopt;
    return cell->get().text;
}

const DataStorage::BlockMap& DataStorage::getRawStorage() const noexcept {
    return m_blocks; 
}

std::optional<std::reference_wrapper<const DataStorage::SpreadsheetBlock::Cell>>
DataStorage::cellAtPos(const Pos& pos) const noexcept {
    auto it = m_blocks.find(Pos(pos.col/SPREADSHEET_BLOCK_SIDE, pos.row/SPREADSHEET_BLOCK_SIDE));
    // check if block that contains pos exists
    if (it == m_blocks.end())
        return std::nullopt;
    
    Pos bpos(pos.col%SPREADSHEET_BLOCK_SIDE, pos.row%SPREADSHEET_BLOCK_SIDE);
    return std::cref(it->second.m_data[bpos.col*SPREADSHEET_BLOCK_SIDE + bpos.row]);
}

std::optional<std::reference_wrapper<DataStorage::SpreadsheetBlock::Cell>>
DataStorage::cellAtPos(const Pos& pos) noexcept {
    auto it = m_blocks.find(Pos(pos.col/SPREADSHEET_BLOCK_SIDE, pos.row/SPREADSHEET_BLOCK_SIDE));
    // check if block that contains pos exists
    if (it == m_blocks.end())
        return std::nullopt;

    Pos bpos(pos.col%SPREADSHEET_BLOCK_SIDE, pos.row%SPREADSHEET_BLOCK_SIDE);
    return std::ref(it->second.m_data[bpos.col*SPREADSHEET_BLOCK_SIDE + bpos.row]);
}

void DataStorage::setData(std::string&& newText, const Pos& pos) {
    // FIXME string_view lifetime problem when assigning a new string
    // if there is no block, insert one
    Pos newBlockKey = {pos.col/SPREADSHEET_BLOCK_SIDE, pos.row/SPREADSHEET_BLOCK_SIDE};
    m_blocks.try_emplace(newBlockKey, SpreadsheetBlock());

    auto cell = cellAtPos(pos);
    if(cell) {
        // initialize raw data in the cell
        cell->get().text = std::move(newText); 
        // parse raw data
        auto formattedVal = parseRawData(cell->get().text); 
        // if a cell already has some format and new format matches -> assign
        if (std::holds_alternative<std::monostate>(cell->get().formattedView)
                || cell->get().formattedView.index() == formattedVal.index())
            cell->get().formattedView = formattedVal;
        else // old and new formats don't match
            cell->get().formattedView = FormatMismatch{};
    } 
    // TODO think of how to deal with !cell (and when it may occur)
}

DataStorage::Format DataStorage::parseRawData(std::string_view text) const {   
    if (text.empty()) return Format{std::monostate{}};
    if (auto val = FormatParser::isFormula(text)) return Format{*val};
    if (auto val = FormatParser::isReal(text)) return Format{*val};
    if (auto val = FormatParser::isInteger(text)) return Format{*val};
    if (auto val = FormatParser::isPercent(text)) return Format{*val};
    if (auto val = FormatParser::isBoolean(text)) return Format{*val};
    return Format{text}; 
    // FIXME extra allocation here is mentally unbearable 
}

void DataStorage::insertColumn(size_t col) {
    BlockMap newMap;
    newMap.reserve(m_blocks.size() * 2);
    
    // the block to insert a column into
    auto C = col / SPREADSHEET_BLOCK_SIDE;
    // offset of a column to insert to inside the block
    auto blockOffset = col % SPREADSHEET_BLOCK_SIDE;

    BlockMapView rowView = buildView(true, true);
    for (auto&& [rowNum, cols] : rowView) {
        // deliberately in the loop, because it'd need to be filled with empty Cells anyways
        BlockSideType carryOver{};
        size_t i = 0;
        size_t sz = cols.size();
        
        // copy the unmodified blocks before the block where insertion occurs
        for (; i < sz && cols[i].first < C; ++i)
            newMap.emplace(Pos(cols[i].first, rowNum), *(cols[i].second));
        // insert an empty array to the correct block
        if (i < sz && cols[i].first == C) {
            newMap.emplace(Pos(cols[i].first, rowNum), shiftRightColsInBlock(carryOver, cols[i].second, blockOffset));
            tryEmplaceAfterShift(newMap, carryOver, Pos(cols[i++].first + 1, rowNum), 1, false);
        }
        // propagate the shift through the rest of the blocks
        while (i < sz && cols[i].first > C) {
            newMap.emplace(Pos(cols[i].first, rowNum), shiftRightColsInBlock(carryOver, cols[i].second, 0));
            tryEmplaceAfterShift(newMap, carryOver, Pos(cols[i++].first + 1, rowNum), 1, false);
        }
    }
    std::swap(m_blocks, newMap);
}

void DataStorage::insertRow(size_t row) {
    BlockMap newMap;
    newMap.reserve(m_blocks.size()*2);

    // the block to insert a row into
    auto R = row / SPREADSHEET_BLOCK_SIDE;
    // offset of a row to insert to inside the block
    auto blockOffset = row % SPREADSHEET_BLOCK_SIDE;

    BlockMapView colView = buildView(false, true);
    for (auto&& [colNum, rows] : colView) {
        // deliberately in the loop, because it'd need to be filled with empty Cells anyways
        BlockSideType carryOver{};
        size_t i = 0;
        size_t sz = rows.size();
        // copy the unmodified blocks before the block where insertion occurs
        for (; i < sz && rows[i].first < R; ++i)
            newMap.emplace(Pos(colNum, rows[i].first), *(rows[i].second));
        // insert an empty array to the correct block
        if (i < sz && rows[i].first == R) {
            newMap.emplace(Pos(colNum, rows[i].first), shiftRightRowsInBlock(carryOver, rows[i].second, blockOffset));
            tryEmplaceAfterShift(newMap, carryOver, Pos(colNum, rows[i++].first + 1), SPREADSHEET_BLOCK_SIDE, false);
        }
        // propagate the shift through the rest of the blocks
        while (i < sz && rows[i].first > R) {
            newMap.emplace(Pos(colNum, rows[i].first), shiftRightRowsInBlock(carryOver, rows[i].second, 0));
            tryEmplaceAfterShift(newMap, carryOver, Pos(colNum, rows[i++].first + 1), SPREADSHEET_BLOCK_SIDE, false);
        }
    }
    std::swap(m_blocks, newMap);
}

void DataStorage::deleteColumn(size_t col) {
    BlockMap newMap;
    newMap.reserve(m_blocks.size() * 2);

    // the block to delete a column in
    auto C = col / SPREADSHEET_BLOCK_SIDE;
    // offset of a column to delete inside the block
    auto blockOffset = col % SPREADSHEET_BLOCK_SIDE;

    BlockMapView rowView = buildView(true, false);
    for (auto&& [rowNum, cols] : rowView) {
        // deliberately in the loop, because it'd need to be filled with empty Cells anyways
        BlockSideType carryOver{};
        size_t i = 0;
        size_t sz = cols.size();

        // propagate the shift through the blocks after R
        while (i < sz && cols[i].first > C) {
            newMap.emplace(Pos(cols[i].first, rowNum), shiftLeftColsInBlock(carryOver, cols[i].second, 0));
            tryEmplaceAfterShift(newMap, carryOver, Pos(cols[i++].first - 1, rowNum), 1, true);
        }       
        // delete the chosen row 
        if (i < sz && cols[i].first == C) {
            newMap.emplace(Pos(cols[i].first, rowNum), shiftLeftColsInBlock(carryOver, cols[i].second, blockOffset));
            tryEmplaceAfterShift(newMap, carryOver, Pos(cols[i++].first - 1, rowNum), 1, true);
        }
        // copy the unmodified blocks before the block where deletion occurs
        for (; i < sz && cols[i].first < C; ++i)
            newMap.emplace(Pos(cols[i].first, rowNum), *(cols[i].second));
    }
    std::swap(m_blocks, newMap);
}

void DataStorage::deleteRow(size_t row) {
    BlockMap newMap;
    newMap.reserve(m_blocks.size() * 2);

    // the block to delete a row in
    auto R = row / SPREADSHEET_BLOCK_SIDE;
    // offset of a row to delete inside the block
    auto blockOffset = row % SPREADSHEET_BLOCK_SIDE;

    BlockMapView colView = buildView(false, false);
    for (auto&& [colNum, rows] : colView) {
        // deliberately in the loop, because it'd need to be filled with empty Cells anyways
        BlockSideType carryOver{};
        size_t i = 0;
        size_t sz = rows.size();

        // propagate the shift through the blocks after R
        while (i < sz && rows[i].first > R) {
            newMap.emplace(Pos(colNum, rows[i].first), shiftLeftRowsInBlock(carryOver, rows[i].second, 0));
            tryEmplaceAfterShift(newMap, carryOver, Pos(colNum, rows[i++].first - 1), SPREADSHEET_BLOCK_SIDE, true);
        }
        // delete the chosen row 
        if (i < sz && rows[i].first == R) {
            newMap.emplace(Pos(colNum, rows[i].first), shiftLeftRowsInBlock(carryOver, rows[i].second, blockOffset));
            tryEmplaceAfterShift(newMap, carryOver, Pos(colNum, rows[i++].first - 1), SPREADSHEET_BLOCK_SIDE, true);
        }
        // copy the unmodified blocks before the block where deletion occurs
        for (; i < sz && rows[i].first < R; ++i)
            newMap.emplace(Pos(colNum, rows[i].first), *(rows[i].second));
    }
    std::swap(m_blocks, newMap);
}

void DataStorage::clear() {
    m_blocks.clear();
}

void DataStorage::swap(BlockMap&& otherMap) noexcept {
    std::swap(m_blocks, otherMap);
}

DataStorage::SpreadsheetBlock DataStorage::shiftRightColsInBlock(BlockSideType& carryOver, const SpreadsheetBlock* const block, const size_t blockOffset) {
    SpreadsheetBlock newBlock;
    // copy columns left of the given position 
    for (size_t c = 0; c < blockOffset; ++c) 
        for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r + c*SPREADSHEET_BLOCK_SIDE];

    // given postion 
    for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r)
        newBlock.m_data[r + blockOffset*SPREADSHEET_BLOCK_SIDE] = carryOver[r];

    // shift the columns right of the given position
    for (size_t c = blockOffset + 1; c < SPREADSHEET_BLOCK_SIDE; ++c) 
        for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r + (c-1)*SPREADSHEET_BLOCK_SIDE];

    // fill the carry with the rightmost column
    for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r)
        carryOver[r] = block->m_data[r + (SPREADSHEET_BLOCK_SIDE-1)*SPREADSHEET_BLOCK_SIDE];
    return newBlock;
}

DataStorage::SpreadsheetBlock DataStorage::shiftRightRowsInBlock(BlockSideType& carryOver, const SpreadsheetBlock* const block, const size_t blockOffset) {
    SpreadsheetBlock newBlock;
    // copy rows left of the given position 
    for (size_t r = 0; r < blockOffset; ++r) 
        for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r + c*SPREADSHEET_BLOCK_SIDE];

    // given position
    for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c)
        newBlock.m_data[blockOffset + c*SPREADSHEET_BLOCK_SIDE] = carryOver[c];

    // shift the rows right of the given position
    for (size_t r = blockOffset + 1; r < SPREADSHEET_BLOCK_SIDE; ++r) 
        for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r-1 + c*SPREADSHEET_BLOCK_SIDE];

    // fill the carry with the rightmost row 
    for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c)
        carryOver[c] = block->m_data[SPREADSHEET_BLOCK_SIDE-1 + c*SPREADSHEET_BLOCK_SIDE];
    return newBlock;
}


DataStorage::SpreadsheetBlock DataStorage::shiftLeftColsInBlock(BlockSideType& carryOver, const SpreadsheetBlock* const block, const size_t blockOffset) {
    SpreadsheetBlock newBlock;
    
    // shift the columns left of the given position
    for (size_t c = 0; c < blockOffset; ++c) 
        for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r + c*SPREADSHEET_BLOCK_SIDE];

    // shift the columns right of the given position
    for (size_t c = blockOffset; c < SPREADSHEET_BLOCK_SIDE - 1; ++c) 
        for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r + (c+1)*SPREADSHEET_BLOCK_SIDE];
    
    // the given position
    for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r)
        newBlock.m_data[r + (SPREADSHEET_BLOCK_SIDE-1)*SPREADSHEET_BLOCK_SIDE] = carryOver[r];

    // fill the carry with the leftmost columns 
    for (size_t r = 0; r < SPREADSHEET_BLOCK_SIDE; ++r)
        carryOver[r] = block->m_data[r];
    return newBlock;
}

DataStorage::SpreadsheetBlock DataStorage::shiftLeftRowsInBlock(BlockSideType& carryOver, const SpreadsheetBlock* const block, const size_t blockOffset) {
    SpreadsheetBlock newBlock;

    // shift the rows left of the given position
    for (size_t r = 0; r < blockOffset; ++r) 
        for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r + c*SPREADSHEET_BLOCK_SIDE];

    // shift the rows right of the given position
    for (size_t r = blockOffset; r < SPREADSHEET_BLOCK_SIDE - 1; ++r) 
        for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c) 
            newBlock.m_data[r + c*SPREADSHEET_BLOCK_SIDE] = block->m_data[r+1 + c*SPREADSHEET_BLOCK_SIDE];

    // the given position
    for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c)
        newBlock.m_data[SPREADSHEET_BLOCK_SIDE-1 + c*SPREADSHEET_BLOCK_SIDE] = carryOver[c];

    // fill the carry with the leftmost row
    for (size_t c = 0; c < SPREADSHEET_BLOCK_SIDE; ++c)
        carryOver[c] = block->m_data[c*SPREADSHEET_BLOCK_SIDE];
    return newBlock;
}

DataStorage::BlockMapView DataStorage::buildView(bool isRowView, bool isAscending) {
    BlockMapView view;
    view.reserve(m_blocks.size());
    if (isRowView) { // build row view
        for (auto&& [key, block] : m_blocks)
            view[key.row].push_back({key.col, &block}); 
    } else { // build column view
        for (auto&& [key, block] : m_blocks)
            view[key.col].push_back({key.row, &block}); 
    }

    if (isAscending) { // sort the view in ascending order
        for (auto& [key, vec] : view) {
            std::sort(vec.begin(), vec.end(),
                [](auto& a, auto& b) { return a.first < b.first; });
        }
    } else { // sort the view in descending order
        for (auto& [key, vec] : view) {
            std::sort(vec.begin(), vec.end(),
                [](auto& a, auto& b) { return a.first > b.first; });
        }
    }
    return view;
}

void DataStorage::tryEmplaceAfterShift(BlockMap& newMap, BlockSideType& carry, const Pos& pos, size_t rowOffset, bool isDelete) {
    // the carry is not empty and there is no block ahead, create a new block 
    if (!([carry]{ for (auto&& cell : carry) if (!cell.text.empty()) return false; return true;}())
            && !m_blocks.contains(pos)) {
        SpreadsheetBlock newBlock;
        for (size_t j = 0; j < SPREADSHEET_BLOCK_SIDE; ++j) 
            newBlock.m_data[isDelete*(SPREADSHEET_BLOCK_SIDE-1)*SPREADSHEET_BLOCK_SIDE/rowOffset + j*rowOffset]
                = carry[j];
        carry = {};
        newMap.emplace(pos, std::move(newBlock));
    }
}


