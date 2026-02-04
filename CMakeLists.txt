cmake_minimum_required(VERSION 3.20)

# C++ FLAGS
set(CMAKE_CXX_COMPILER "g++" CACHE STRING "C++ compiler" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)   
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("Spreadsheet")
find_package(PkgConfig REQUIRED)

# COMPILER FLAGS
set(BASE_FLAGS
    -std=c++23
)
set(STRICT_FLAGS
    -Wstrict-aliasing
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wwrite-strings
    -Wconversion
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Wfloat-equal
    -Wsign-compare
    -Wundef
    -Wcast-align
    -Wswitch-default
    -Wimplicit-fallthrough
    -Wempty-body
    -Wuninitialized
    -Wmisleading-indentation
    -Wshadow
)   
set(DEBUG_FLAGS 
    -O0
    -g3
    -ggdb3
    -fno-strict-aliasing
    -fno-elide-constructors
    -fno-omit-frame-pointer
    # -fdiagnostics-show-template-tree
    -fstack-protector-strong
    # -fdump-tree-all-graph
    # -fdump-tree-all-graph-raw
	-DDEBUG
    # -fsanitize=address
    # -fsanitize-address-use-after-scope
	-ftrapv
)
set(RELEASE_FLAGS
    -O3
    -march=native
    -flto=auto
    -fno-plt
    -fno-omit-frame-pointer
)

# LIBRARIES
# set(DEBUG_LIBS 
#     -fsanitize=address
#     -fsanitize-address-use-after-scope
# )
set(RELEASE_LIBS -flto)

# DEPENDDENCIES
pkg_check_modules(DEPS REQUIRED IMPORTED_TARGET GLOBAL sdl3 sdl3-image sdl3-ttf)

add_executable (Spreadsheet "src/main.cpp" "src/Application.cpp" "src/GUI.cpp" "src/TextRenderer.cpp"
    "src/ILayer.cpp"  
    "src/TableHandlers.cpp" "src/TaskBarHandlers.cpp"
    "src/DataStorage.cpp" "src/SpreadsheetSerializer.cpp" "src/Pos.cpp"
    "src/TableStateMachine.cpp" 
    "src/TableWidget.cpp" "src/Widget.cpp")
target_compile_options(Spreadsheet PRIVATE ${BASE_FLAGS} ${STRICT_FLAGS} ${DEBUG_FLAGS})
target_include_directories(Spreadsheet PRIVATE ${CMAKE_SOURCE_DIR}/headers ${DEPS_INCLUDE_DIRS})
add_compile_options(-fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.)   
   
target_link_libraries(Spreadsheet PRIVATE PkgConfig::DEPS
    # $<$<CONFIG:Debug>:${DEBUG_LIBS}>
    $<$<CONFIG:Release>:${RELEASE_LIBS}>
)   

