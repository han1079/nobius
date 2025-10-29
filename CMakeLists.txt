cmake_minimum_required(VERSION 3.10)
project(CANIMATE CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXT_SDL_SUBMODULE_PATH ${CMAKE_SOURCE_DIR}/ext/SDL)

add_custom_target(build_sdl2
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/include
    COMMAND ${CMAKE_COMMAND} -E chdir ${EXT_SDL_SUBMODULE_PATH}
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=ON
    COMMAND ${CMAKE_COMMAND} --build ${EXT_SDL_SUBMODULE_PATH}/build --parallel
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXT_SDL_SUBMODULE_PATH}/build/libSDL2-2.0.so
        ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib/libSDL2-2.0.so
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${EXT_SDL_SUBMODULE_PATH}/build/include/SDL2
        ${CMAKE_SOURCE_DIR}/ext/PortaSDL2
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXT_SDL_SUBMODULE_PATH}/build/include-config-release/SDL2/SDL_config.h
        ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/SDL_config.h
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib/libSDL2-2.0.so
        ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib/libSDL2.so
    COMMENT "Building and copying SDL2 to ext/PortaSDL2/"
)


set(IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/ext/imgui/imgui.cpp
                        ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_draw.cpp
                        ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_tables.cpp
                        ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_widgets.cpp
                        ${CMAKE_SOURCE_DIR}/ext/imgui/backends/imgui_impl_sdl2.cpp
                        ${CMAKE_SOURCE_DIR}/ext/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui_sanity_check ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_demo.cpp ${IMGUI_SOURCES})

target_include_directories(imgui_sanity_check PRIVATE
    ${CMAKE_SOURCE_DIR}/ext 
    ${CMAKE_SOURCE_DIR}/ext/imgui 
    ${CMAKE_SOURCE_DIR}/ext/imgui/backends
    ${CMAKE_SOURCE_DIR}/ext/PortaSDL2
    ${CMAKE_SOURCE_DIR}/ext/nlohmann_json/single_include)

target_link_directories(imgui_sanity_check PRIVATE
    ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib)

target_link_libraries(imgui_sanity_check PRIVATE dl GL SDL2 pthread)


