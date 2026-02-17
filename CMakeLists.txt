cmake_minimum_required(VERSION 3.20)

project(IdleScreen CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(IdleScreen WIN32
        src/main.cpp
        src/win/BlackOverlay_win.cpp
)

target_include_directories(IdleScreen
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (WIN32)
    target_compile_definitions(IdleScreen PRIVATE
            UNICODE
            _UNICODE
            WIN32_LEAN_AND_MEAN
    )
else()
    message(FATAL_ERROR "This project currently supports Windows only.")
endif()
