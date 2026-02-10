# executables
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable (Spreadsheet ${SRC})

# features 
target_compile_features(Spreadsheet PUBLIC cxx_std_23)

# options
target_compile_options(Spreadsheet PRIVATE
    -Wstrict-aliasing
    -Wall 
    -Wextra 
    -Werror
    -Wpedantic 
    -Wwrite-strings
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Wfloat-equal
    -Wcast-align
    -Wimplicit-fallthrough
    -Wempty-body
    -Wuninitialized
    -Wmisleading-indentation
    -Wshadow
    -Wconversion 
    -Wsign-compare
    -Wswitch-default 
    -Wundef
)

if (GUI_ENABLE_LTO)
    set_property(TARGET Spreadsheet PROPERTY
        INTERPROCEDURAL_OPTIMIZATION_RELEASE true)
endif()

target_compile_options(Spreadsheet PRIVATE
    $<$<CONFIG:DEBUG>:
        -O0
        -g3
        -ggdb3
        -fno-elide-constructors
        -fno-strict-aliasing
        -fno-omit-frame-pointer
        -ftrapv
        -fstack-protector-strong
    >
)

target_compile_options(Spreadsheet PRIVATE
    $<$<CONFIG:RELEASE>:
        -O3
        -march=native
        -fno-plt
    >
)

# definitions
target_compile_definitions(Spreadsheet PRIVATE
    $<$<CONFIG:DEBUG>:DEBUG>
)

target_include_directories(Spreadsheet
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

add_compile_options(-fdebug-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.)   

target_link_libraries(Spreadsheet PRIVATE RTWgui)

# target_link_libraries(Spreadsheet PRIVATE ${GUI_LIBS}
#     # $<$<CONFIG:Debug>:${DEBUG_LIBS}>
#     $<$<CONFIG:Release>:${RELEASE_LIBS}>
# )   

