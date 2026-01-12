#pragma once
#include <pch.h>
#include <core/common.h>


struct WindowState {
    bool visible = false;
    std::string window_name = "Default";
    ImVec2 pos = {0,0};
    ImVec2 size = {0,0};

    ImVec2 initial_pos = {0,0};
    ImVec2 initial_size = {0,0};
    std::unordered_map<std::string, std::string> texts = {};
};

namespace UI {
    void initialize_imgui(SDL_Window* window, SDL_GLContext gl_context, float main_scale);
    void shutdown_imgui();
    void push_event_to_imgui(const SDL_Event* event);
    void update_imgui_state();
    void build_main_window(WindowState& state);
    void build_sidebar(WindowState& state);
    void build_bottom_panel(WindowState& state);
    void build_ribbon(WindowState& state);
    void build_debug_console(WindowState& state);
}
