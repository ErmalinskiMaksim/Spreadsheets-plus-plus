include(FetchContent)

if(GRAPHICS_LIBRARY STREQUAL "SDL")
    # --- SDL3 ---
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        main 
    )

    # --- SDL3_ttf ---
    FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG        main
    )

    # Build settings for dependencies
    set(SDL3_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL3_EXAMPLES OFF CACHE BOOL "" FORCE)

    # Make the packages available
    FetchContent_MakeAvailable(SDL3 SDL3_ttf)
    
elseif(GRAPHICS_LIBRARY STREQUAL "SFML")
    # --- SFML ---
    FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1 
    )

    # Build settings for dependencies
    set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(SFML_BUILD_DOCS OFF CACHE INTERNAL "")
    set(SFML_INSTALL OFF CACHE INTERNAL "")

    # Make the packages available
    FetchContent_MakeAvailable(SFML)
endif()
