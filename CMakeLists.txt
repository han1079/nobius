cmake_minimum_required(VERSION 3.10)
project(CANIMATE CXX C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile_commands.json for clangd")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2)
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets)
qt6_standard_project_setup()

add_executable(basic_draw ${CMAKE_SOURCE_DIR}/app/basic_draw.cpp ${CMAKE_SOURCE_DIR}/ext/glad/src/glad.c)
target_link_libraries(basic_draw ${SDL2_LIBRARIES})
target_include_directories(basic_draw PRIVATE ${CMAKE_SOURCE_DIR}/ext/glad/include ${SDL2_INCLUDE_DIRS})

add_executable(mouse_tracker ${CMAKE_SOURCE_DIR}/src/app/mouse_tracker.cpp 
                            ${CMAKE_SOURCE_DIR}/src/ui/control_panel.cpp
                            )
target_include_directories(mouse_tracker PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/ext)
target_link_libraries(mouse_tracker Qt6::Core Qt6::Gui Qt6::Widgets)
target_compile_options(mouse_tracker PRIVATE -g -O2 -DDEBUG)



