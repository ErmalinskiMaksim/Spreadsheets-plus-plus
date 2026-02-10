cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)   
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Spreadsheet LANGUAGES CXX)

# optional library
set(GRAPHICS_LIBRARY "SDL" CACHE STRING "The graphics library to use (SDL or SFML)")

# Create a preprocessor definition based on the choice
if(GRAPHICS_LIBRARY STREQUAL "SDL")
    add_compile_definitions(USE_SDL)
elseif(GRAPHICS_LIBRARY STREQUAL "SFML")
    add_compile_definitions(USE_SFML)
else()
    message(FATAL_ERROR "Invalid GRAPHICS_LIBRARY: ${GRAPHICS_LIBRARY}. Choose SDL or SFML.")
endif()

include(cmake/dependencies.cmake)
# Include sub-projects.
add_subdirectory ("Spreadsheets-plus-plus")
