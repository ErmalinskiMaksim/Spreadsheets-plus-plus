#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "SpreadsheetSerializer.h"
#include <fstream>

// The main file managing class
class FileManager {
    friend class FileActionHandler;
    friend consteval auto buildFilePathDialogPayload();
    static inline constexpr uint8_t MAX_FILE_PATH_LEN = 64;
    // save spreadsheet to file
    static bool saveSpreadsheet(const std::string& filePath, const Serializer& serializer) {
        std::ofstream file(filePath);
        if (!file) return false;
        serializer.serialize(DataStorage::get().getRawStorage(), file);
        file.close();
        return true;
    } // load spreadsheet from file
    static bool loadSpreadsheet(const std::string& filePath, const Serializer& serializer) {
        std::ifstream file(filePath);
        if (!file) return false;
        serializer.deserialize(file);
        file.close();
        return true;
    }
};

#endif // !FILE_MANAGER_H
