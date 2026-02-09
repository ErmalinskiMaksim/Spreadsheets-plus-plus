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

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable (Spreadsheet ${SRC})
target_compile_options(Spreadsheet PRIVATE ${BASE_FLAGS} ${STRICT_FLAGS})
target_include_directories(Spreadsheet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/headers)
add_compile_options(-fdebug-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.)   
   
target_link_libraries(Spreadsheet PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf
    # $<$<CONFIG:Debug>:${DEBUG_LIBS}>
    $<$<CONFIG:Release>:${RELEASE_LIBS}>
)   

