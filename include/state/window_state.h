#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/base_state.h>

class WindowState : public BaseState {
public:
    WindowState() = default;
    ~WindowState() = default;
private:


    // SDL and OpenGL configuration parameters
    int sdl_flags = 0;
    SDL_GLprofile sdl_profile_mask = SDL_GL_CONTEXT_PROFILE_CORE;
    int sdl_major_version = 3;
    int sdl_minor_version = 0;
    int sdl_double_buffer = 1;

    int sdl_depth_size = 24;
    int sdl_stencil_size = 8;
    SDL_WindowFlags sdl_window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    int sdl_window_width = 1280;
    int sdl_window_height = 800;
    std::string sdl_window_title = "Placeholder Title";

    float imgui_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
    ImGuiConfigFlags imgui_config_flags = (ImGuiConfigFlags_NavEnableKeyboard | 
                                            ImGuiConfigFlags_DockingEnable | 
                                            ImGuiConfigFlags_ViewportsEnable);

    std::string glsl_version = "#version 130";
    std::string imgui_style = "dark";
    
    glm::vec4 gl_clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

    friend class Renderer;
    friend class Orchestrator;
};
