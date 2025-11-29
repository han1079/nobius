#pragma once

#include <pch.h>
#include <core/common.h>
#include <core/base_updater.h>
#include <state/imgui_state.h>


class ImGuiUpdater : public BaseUpdater {
public:

    
    ImGuiUpdater() = delete; // ImGuiUpdater MUST have a state in order to be created.
    ImGuiUpdater(ImGuiState& state);
    ~ImGuiUpdater();
    friend class Orchestrator;
    
    // Getter for window access
    SDL_Window* get_window() const { return window; }

private:
    ImGuiState& m_cfg;

    // Basic Window Configuration Data
    
    int win_width;
    int win_height;
    SDL_Window* window;
    SDL_GLContext gl_context;
    float main_scale;


    // Stuff for debugging imgui
    ImGuiStyle style;
protected:

    bool init() override;
    bool shutdown() override;
    bool update_state_via_event(EngineEvent &event) override;
    bool update_state_via_dT(float dT) override;

private:

    void build_imgui_frame();
    void initialize_imgui_frame();
    bool draw_gui();
};
