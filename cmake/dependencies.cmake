include(FetchContent)

# --- SDL3 ---
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        main # Or a specific tag like release-3.2.0
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
