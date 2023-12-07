include(FetchContent)

set(SDL_VERSION 2.28.5)

FetchContent_Declare(
    SDL
    URL "https://github.com/libsdl-org/SDL/archive/release-${SDL_VERSION}.zip"
)

FetchContent_MakeAvailable(SDL)
