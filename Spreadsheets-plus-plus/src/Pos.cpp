#include "Pos.h"

// for hash map
bool Pos::operator<(const Pos& other) const {
    return (row < other.row) || (row == other.row && col < other.col);
}
