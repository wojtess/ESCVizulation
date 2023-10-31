cmake_minimum_required(VERSION 3.9.1)

project(esc)

find_package(raylib 3.0 REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

add_executable(esc
    src/main.cpp
    src/engine.cpp
    src/window.cpp
    src/renderers.cpp

    libs/rlImGui/rlImGui.cpp

    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_demo.cpp

    libs/implot/implot_items.cpp
    libs/implot/implot.cpp
    libs/implot/implot_demo.cpp
)

target_include_directories(esc PUBLIC
    headers
    libs/rlImGui
    libs/imgui
    libs/implot
)

target_link_libraries(esc
    raylib
)