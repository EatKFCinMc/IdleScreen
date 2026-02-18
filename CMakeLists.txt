cmake_minimum_required(VERSION 3.20)

project(IdleScreen CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(IdleScreen)

target_sources(IdleScreen PRIVATE
        src/main.cpp
)

target_include_directories(IdleScreen PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (WIN32)
    set_target_properties(IdleScreen PROPERTIES WIN32_EXECUTABLE TRUE)

    target_sources(IdleScreen PRIVATE
            src/win/overlay_win.cpp
    )

    target_compile_definitions(IdleScreen PRIVATE
            UNICODE
            _UNICODE
            WIN32_LEAN_AND_MEAN
    )

elseif(APPLE)
    message(FATAL_ERROR "Support of MacOS haven't been add yet.")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(IdleScreen PRIVATE
            src/linux/overlay_linux.cpp
    )

else()
    message(FATAL_ERROR "Unsupported platform")
endif()
