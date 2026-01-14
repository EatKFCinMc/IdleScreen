cmake_minimum_required(VERSION 3.20)

project(BlackScreenIdleWatcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(BlackScreenIdleWatcher WIN32
    src/main.cpp
    src/BlackOverlay.cpp
)

if(MINGW)
    # Ensure the wide-character entry point (wWinMain) is used.
    target_link_options(BlackScreenIdleWatcher PRIVATE -municode)
endif()

target_include_directories(BlackScreenIdleWatcher PRIVATE include)

target_compile_definitions(BlackScreenIdleWatcher PRIVATE
    UNICODE
    _UNICODE
)

if(MSVC)
    target_compile_options(BlackScreenIdleWatcher PRIVATE /W4 /permissive-)
else()
    target_compile_options(BlackScreenIdleWatcher PRIVATE -Wall -Wextra -Wpedantic)
endif()
