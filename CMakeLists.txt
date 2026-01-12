cmake_minimum_required(VERSION 3.10)
project(CANIMATE CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug configuration for GDB
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
    message(STATUS "Building in Debug mode for GDB")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

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
                        ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_demo.cpp
)

set(CORE_BUILD ${CMAKE_SOURCE_DIR}/src/updaters/base_updater.cpp
                ${CMAKE_SOURCE_DIR}/src/core/vertex_allocator.cpp
                ${CMAKE_SOURCE_DIR}/src/core/entity_manager.cpp
                ${CMAKE_SOURCE_DIR}/src/core/common.cpp
                ${CMAKE_SOURCE_DIR}/src/core/entity.cpp
                ${CMAKE_SOURCE_DIR}/src/core/shader.cpp
                ${CMAKE_SOURCE_DIR}/src/core/orchestrator.cpp
                ${CMAKE_SOURCE_DIR}/src/updaters/world_updater.cpp
                ${CMAKE_SOURCE_DIR}/src/updaters/input_system.cpp
                ${CMAKE_SOURCE_DIR}/src/updaters/imgui_updater.cpp
                ${CMAKE_SOURCE_DIR}/src/core/renderer.cpp
                ${CMAKE_SOURCE_DIR}/src/utils/debug_console.cpp)

set(APP_INCLUDE_PATHS ${CMAKE_SOURCE_DIR}/include
                       ${CMAKE_SOURCE_DIR}/ext 
                       ${CMAKE_SOURCE_DIR}/ext/glm
                       ${CMAKE_SOURCE_DIR}/ext/imgui 
                       ${CMAKE_SOURCE_DIR}/ext/imgui/backends
                       ${CMAKE_SOURCE_DIR}/ext/PortaSDL2
                       ${CMAKE_SOURCE_DIR}/ext/glad/include
                       ${CMAKE_SOURCE_DIR}/ext/nlohmann_json/single_include)


add_library(imgui_sanity_check ${CMAKE_SOURCE_DIR}/ext/imgui/imgui_demo.cpp 
                               ${CMAKE_SOURCE_DIR}/src/core/common.cpp 
                               ${IMGUI_SOURCES})
                               
target_include_directories(imgui_sanity_check PRIVATE ${APP_INCLUDE_PATHS})
target_precompile_headers(imgui_sanity_check PRIVATE ${CMAKE_SOURCE_DIR}/include/pch.h)

# Exclude ImGui source files themselves from using PCH
set_source_files_properties(${IMGUI_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_link_directories(imgui_sanity_check PRIVATE ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib)
target_link_libraries(imgui_sanity_check PRIVATE dl GL SDL2 pthread)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/ext/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/ext/glad/include)
target_link_libraries(glad PRIVATE dl GL)
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)


add_executable(nobius_app ${CMAKE_SOURCE_DIR}/src/app/main_app.cpp 
                               ${CORE_BUILD} 
                               ${IMGUI_SOURCES}
                               ${VENDOR_SOURCES})
target_compile_definitions(nobius_app PRIVATE -DPROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")
target_include_directories(nobius_app PRIVATE ${APP_INCLUDE_PATHS})
target_precompile_headers(nobius_app PRIVATE ${CMAKE_SOURCE_DIR}/include/pch.h)

# Exclude ImGui source files from using PCH
set_source_files_properties(${IMGUI_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

target_link_directories(nobius_app PRIVATE ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib)
target_link_libraries(nobius_app PRIVATE glad dl GL SDL2 pthread)

# add_executable(renderer_test ${CMAKE_SOURCE_DIR}/src/app/renderer_test.cpp
#                             ${CMAKE_SOURCE_DIR}/src/core/common.cpp
#                             ${CMAKE_SOURCE_DIR}/src/core/renderer.cpp
#                             ${CMAKE_SOURCE_DIR}/src/core/entity.cpp
#                             ${VENDOR_SOURCES})


# target_include_directories(renderer_test PRIVATE ${APP_INCLUDE_PATHS})
# target_precompile_headers(renderer_test PRIVATE ${CMAKE_SOURCE_DIR}/include/pch.h)
# target_link_directories(renderer_test PRIVATE ${CMAKE_SOURCE_DIR}/ext/PortaSDL2/lib)
# target_link_libraries(renderer_test PRIVATE glad dl GL SDL2 pthread)

