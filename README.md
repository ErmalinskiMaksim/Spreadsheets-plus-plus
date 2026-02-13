# Spreadsheets++ (Spreadsheets-plus-plus)
## 1.  Description
This is a basic spreadsheets application made in C++ and RTWgui. You can edit the spreadsheet and perform basic operations on data.

## 2. Dependencies
The application is powered by [RTWgui](https://github.com/ErmalinskiMaksim/RTWgui).

## 3. Platforms 
Supports Linux and Windows.

## 4. User guide 
At the moment, the user can:
* write data to the spreadsheet
* resize columns/rows by holding LCTRL+LMB
* insert/delete columns/rows by right clicking their labels
* open/save/close files
Advice can be found in the Help menu inside the app

## 5. Build&Run
```bash
# fetch 
git clone https://github.com/ErmalinskiMaksim/Spreadsheets-plus-plus.git
# build 
cd Spreadsheets-plus-plus
cmake -S . -B build
cmake --build build
# run 
build/Spreadsheets-plus-plus/Spreadsheet
```

## 6. Future plans
* Functions
* Tools for the toolbar 
* Plots
