#ifndef SPREADSHEET_SERIALIZER_H
#define SPREADSHEET_SERIALIZER_H

#include "DataStorage.h"

// The main class that is responsible for serializing 
// data storage into a file and performing deserialization
class Serializer {
public:
    void serialize(const DataStorage::BlockMap&, std::ostream&) const;
    void deserialize(std::istream&) const;
};

#endif 
