# Spreadsheets++ (Spreadsheets-plus-plus)
## 1.  Description
This is a basic spreadsheets application made in C++. You can edit the spreadsheet and perform basic operations on data.

## 2. Dependencies
It supports 2 groups of libraries:
1. SDL3, SDL3_TTF
2. SFML
## 3. Platforms 
At the moment, it only supports Linux but Windows support is coming soon.

## 4. User guide 
At the moment, the user can:
* write data to the spreadsheet
* resize columns/rows by holding LCTRL+LMB
* insert/delete columns/rows by right clicking their labels
* open/save/close files
Advice can be found in the Help menu inside the app

## 5. Build&Run
1. Fetch:\
```git clone https://github.com/ErmalinskiMaksim/Spreadsheets-plus-plus.git```
2. Build:\
```cd Spreadsheets-plus-plus```\
```cmake -S . -B build```\
```cmake --build build```
3. Run:\
```build/Spreadsheets-plus-plus/Spreadsheet```
