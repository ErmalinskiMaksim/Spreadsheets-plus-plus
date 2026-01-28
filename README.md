# Spreadsheets++ (Spreadsheets-plus-plus)
## 1.  Description
This is a basic spreadsheets application made in C++. You can edit the spreadsheet and perform basic operations on data.

## 2. Dependencies
It is powered by SDL3 and SDL3_TTF

## 3. Platforms 
At the moment, it only supports Linux but Windows support is coming soon.

## 4. User guide 
At the moment, the user can:
* write data to the spreadsheet
* resize columns/rows by holding LCTRL+LMB
* insert/delete columns/rows by right clicking their labels
* open/save/close files
Advice can be found in the Help menu inside the app

## 5. Build
The project requires two libreries, as was mentioned above. The installation guides:
https://github.com/libsdl-org/SDL
https://github.com/libsdl-org/SDL_ttf

add them to the PATH and run:
cd path/to/the/directory
cmake -S . -B build
cmake --build build
