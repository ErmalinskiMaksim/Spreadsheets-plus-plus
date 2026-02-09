#ifndef POS_H
#define POS_H

#include <functional>

// position format used inside the DataStorage
struct Pos {
    size_t col;
    size_t row;
    bool operator==(const Pos&) const = default;
    bool operator<(const Pos&) const;
};

// a hash function used in DataStorage to enable 
// Pos type to be a key in a hash map
namespace std {
    template<>
    struct hash<Pos> {
        size_t operator() (const Pos& p) const {
		return std::rotl(hash<size_t>{}(p.col),1) ^
			   hash<size_t>{}(p.row);
	    }
    };
}

#endif
