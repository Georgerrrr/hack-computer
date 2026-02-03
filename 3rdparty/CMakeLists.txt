include(FetchContent)

FetchContent_Declare(
  external-SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.32.10
)

FetchContent_MakeAvailable(external-SDL2)

