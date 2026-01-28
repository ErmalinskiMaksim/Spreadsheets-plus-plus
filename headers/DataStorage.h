#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

// This file contains the entities that are responsible 
// for the data storage inside of the spradsheet 

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <string>
#include <charconv>
#include <vector>
#include "Pos.h"

// a helper concept that generalizes arithmetic 
// types in the DataStorage
template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;   

// A data type used in DataStorage that should
// be utilized when when a format of input
// doesn't match with the format that was set 
// before
struct FormatMismatch {
    static constexpr const char* errorMessage = "ERROR";
};

// a percent type in DataStorage
class Percent {
public:
    template<Arithmetic T>
    explicit Percent(T val) noexcept : value(static_cast<double>(val)*100.0) {}
    explicit Percent(double val, char) noexcept : value(val) {}
    template<Arithmetic T>
    explicit operator T() const noexcept { return static_cast<T>(value / 100.0); }
    double getPercents() const noexcept { return value; }
    std::string toString() const noexcept;
private:
    double value;
};

// a formula type in DataStorage
class Formula {};


// The way the data is organized in the DataStorage:
// * Spreadsheet data is stored sparsely inside of a hashmap
// * Key = Pos, Value = SpreadsheetBlock 
// * SpreadsheetBlock is an array<Cell, N>,
// where N = SPREADSHEET_BLOCK_SIDE*SPREADSHEET_BLOCK_SIDE
// SPREADSHEET_BLOCK_SIDE is a "side" of a squre block of 
// Cells. The amount of columns and rows is the same in a block
inline constexpr size_t SPREADSHEET_BLOCK_SIDE = 8;
// Maximum string length inside of a Cell
inline constexpr size_t MAX_CELL_TEXT_LENGTH = 64; 

// Main data storage class
class DataStorage {
    struct SpreadsheetBlock;
    friend class Serializer;
public:
    // format of data in the storage
    using Format = std::variant<std::monostate, FormatMismatch, std::string_view, Formula, double, long long, Percent, bool>;
    // the main data structure of the storage
    using BlockMap = std::unordered_map<Pos, SpreadsheetBlock>;
    // a comfortable column- or row-based view of BlockMap  
    using BlockMapView = std::unordered_map<size_t, std::vector<std::pair<size_t, SpreadsheetBlock*>>>;

    // singleton
    DataStorage(const DataStorage&) = delete;
    DataStorage& operator=(const DataStorage&) = delete;
    DataStorage(DataStorage&&) = delete;
    DataStorage& operator=(DataStorage&&) = delete;
    ~DataStorage() = default;

    // checks if there exists data at a given position 
    bool contains(const Pos&) const noexcept;

    // produces the global position (col, row) of a cell based on its 
    // block-local position and the position of the block
    static Pos getCellAbsolutePos(const Pos& block, size_t cell) noexcept {
        return Pos(cell/SPREADSHEET_BLOCK_SIDE + block.col*SPREADSHEET_BLOCK_SIDE,
                cell%SPREADSHEET_BLOCK_SIDE + block.row*SPREADSHEET_BLOCK_SIDE);
    }

    // get storage
    static DataStorage& get() {
        static DataStorage ds;
        return ds;
    }

    // get formatted view of the cell's content at <pos> or nothing if it's empty
    std::optional<std::reference_wrapper<const DataStorage::Format>>
    getValue(const Pos&) const noexcept;
    
    // get raw string data of a cell at <pos> or nothing if it's empty 
    std::optional<std::string> getRawData(const Pos&) const noexcept;

    // get raw BlockMap  
    const BlockMap& getRawStorage() const noexcept;

    // format and write <data> to a cell at <pos>
    void setData(std::string&&, const Pos&);

    // insert a column/row into the storage
    void insertColumn(size_t);
    void insertRow(size_t);
    // delete a column/row from the storage
    void deleteColumn(size_t);
    void deleteRow(size_t);

    // entirely clear the storage
    void clear();
private:
    // Data block abstraction 
    struct SpreadsheetBlock {
        // Cell abstraction
        struct Cell {
            // raw content
            std::string text{};  
            // view for internal use
            mutable Format formattedView = std::monostate{};
        };
        bool empty() const noexcept; 
        // data format: [ col0 | col1 | col2 | col3 | col4 | col5 | col6 | col7 ]
        using BlockArray = std::array<Cell, SPREADSHEET_BLOCK_SIDE * SPREADSHEET_BLOCK_SIDE>;
        BlockArray m_data;
    };
    
    // parses raw data and produces a formatted view
    struct FormatParser {
        static std::optional<Formula> isFormula(std::string_view) {
            return std::nullopt; 
        }
        static std::optional<double> isReal(std::string_view s) {
            double val;
            auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
            if (ec != std::errc{} || ptr != s.data() + s.size())
                return std::nullopt;
            return val;         
        }
        static std::optional<long long> isInteger(std::string_view s) {
            long long val;
            auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
            if (ec != std::errc{} || ptr != s.data() + s.size())
                return std::nullopt;
            return val;
        }
        static std::optional<Percent> isPercent(std::string_view s) {
            if (s.ends_with('%')) 
                if (auto val = isReal(s.substr(0, s.size() - 1))) return Percent{*val, '%'};
            return std::nullopt;
        }
        static std::optional<bool> isBoolean(std::string_view s) {
            if (s == "true") return true;
            if (s == "false") return false;
            return std::nullopt;
        }
    };

    DataStorage() = default;

    // swap internal block map with a given one
    void swap(BlockMap&&) noexcept; 

    // a single column or a row in a block
    using BlockSideType = std::array<SpreadsheetBlock::Cell, SPREADSHEET_BLOCK_SIDE>;

    // get cell content at the given position 
    std::optional<std::reference_wrapper<const SpreadsheetBlock::Cell>>
    cellAtPos(const Pos&) const noexcept;

    // non-const version
    std::optional<std::reference_wrapper<SpreadsheetBlock::Cell>>
    cellAtPos(const Pos&) noexcept;

    // shift columns/rows inside a <block> right by one with <carry> starting from <offset>
    SpreadsheetBlock shiftRightColsInBlock(BlockSideType&, const SpreadsheetBlock* const, const size_t);
    SpreadsheetBlock shiftRightRowsInBlock(BlockSideType&, const SpreadsheetBlock* const, const size_t);
    // shift columns/rows inside a <block> left by one with <carry> starting from <offset>
    SpreadsheetBlock shiftLeftColsInBlock(BlockSideType&, const SpreadsheetBlock* const, const size_t);
    SpreadsheetBlock shiftLeftRowsInBlock(BlockSideType&, const SpreadsheetBlock* const, const size_t);

    // emplace if, after shifting, the next block is empty but carry is not empty
    void tryEmplaceAfterShift(BlockMap&, BlockSideType&, const Pos&, size_t, bool);

    // build a convenient column- or a row- based view of the block map 
    BlockMapView buildView(bool, bool);

    // format raw data
    Format parseRawData(std::string_view) const;

    BlockMap m_blocks;
};

#endif // !DATA_STORAGE_H
