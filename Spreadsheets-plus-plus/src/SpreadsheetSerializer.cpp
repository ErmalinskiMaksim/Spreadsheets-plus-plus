#include "SpreadsheetSerializer.h"
#include <istream>
#include <ostream>

void Serializer::serialize(const DataStorage::BlockMap& map, std::ostream& out) const {
    out << map.size() << '\n';
    for (auto&& [key, block] : map) {
        out << key.col << ' ' << key.row << '\n';
        for (auto&& cell : block.m_data) {
            out << cell.text.size() << ' ' << cell.text << '\n';
        }
    }
} 

void Serializer::deserialize(std::istream& in) const {
    // TODO add security checks
    DataStorage::BlockMap newMap;
    size_t sz;
    in >> sz;
    newMap.reserve(sz);

    for (size_t i = 0; i < sz; ++i) {
        size_t col, row;
        in >> col >> row;

        DataStorage::SpreadsheetBlock block;
        for (auto& cell : block.m_data) {
            size_t len;
            in >> len;
            in.get(); // consume space/newline
            cell.text.resize(len);
            in.read(cell.text.data(), static_cast<ssize_t>(len));
            in.get(); // consume newline
        }

        newMap.emplace(Pos(col, row), std::move(block));
    }
    
    DataStorage::get().swap(std::move(newMap));
}
