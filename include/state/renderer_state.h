#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/base_state.h>


class RendererState : public BaseState {
public:
    RendererState() = delete;
    RendererState(const std::optional<std::string>& fpath);
    ~RendererState();
    
    void save_imgui_style(const std::string& fpath);

protected:

    void load_from_json(const std::optional<std::string>& fpath = std::nullopt) override;
    void load_sdl_config_from_json(const nlohmann::json& sdl_json);
    void load_imgui_style_from_json(const nlohmann::json& style_json);

private:


    // SDL and OpenGL default configuration parameters
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
                                            ImGuiConfigFlags_DockingEnable);
    //                                        ImGuiConfigFlags_ViewportsEnable);

    std::string glsl_version = "#version 130";
    std::string imgui_style = "dark";
    
    glm::vec4 gl_clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

    bool sidebar_visible = true;
    bool bottom_visible = true;
    bool ribbon_visible = true;
    bool viewport_visible = true;

    friend class Renderer;
    friend class Orchestrator;

private:

    int event_count = 0;
    
    template <typename member_type>
    auto get(member_type member_name) -> decltype(this->*member_name) {
        return (this)->*member_name;
    }

    template <typename member_type, typename value_type>
    void set(member_type member_name, value_type val) {
        (this)->*member_name = val; 
    }


};
