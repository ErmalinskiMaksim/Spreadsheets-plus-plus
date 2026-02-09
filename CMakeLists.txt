cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)   
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Spreadsheet LANGUAGES CXX)
include(cmake/dependencies.cmake)
# Include sub-projects.
add_subdirectory ("Spreadsheets-plus-plus")
