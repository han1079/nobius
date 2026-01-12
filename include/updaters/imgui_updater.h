#pragma once

#include <pch.h>
#include <core/common.h>
#include <updaters/base_updater.h>
#include <utils/imgui_style_loader.h>

struct ImGuiWidgetState {
    bool visible = false;
    ImVec2 pos = {0,0};
    ImVec2 size = {0,0};
    std::unordered_map<std::string, std::string> texts = {};
};

class ImGuiUpdater : public BaseUpdater {
public:

    
    ImGuiUpdater() : sidebar_state(), 
        bottom_panel_state(), 
        ribbon_state(), 
        main_window_state(),
        debug_console_state()
    {
        sidebar_state.visible = true;
        sidebar_state.texts["name"] = "Sidebar";

        bottom_panel_state.visible = true;
        bottom_panel_state.texts["name"] = "Bottom Panel";
        
        ribbon_state.visible = true;
        ribbon_state.texts["name"] = "Ribbon";

        main_window_state.visible = true;
        main_window_state.texts["name"] = "Main Window";

        debug_console_state.visible = true;
        debug_console_state.texts["name"] = "Debug Console";
        DEBUG_HOOK_FUNCTION_NO_TIMER();
        DEBUG_HOOK_VAR_AS(debug_console_state.visible, "DEBUG_CONSOLE_VISIBLE");
    }; 
    ~ImGuiUpdater();
    friend class Orchestrator;
    
    // Getter for window access

public:

    ImGuiWidgetState sidebar_state;

    ImGuiWidgetState bottom_panel_state;
 
    ImGuiWidgetState ribbon_state;

    ImGuiWidgetState main_window_state;

    ImGuiWidgetState debug_console_state;

protected:

    bool init() override;
    bool shutdown() override;
    bool update_state_via_event() override;
    bool update_state_via_dT(float dT) override;
    bool submit_render_commands() override;

private:
    void build_main_window();
    void build_sidebar();
    void build_ribbon();
    void build_bottom_panel();
    void build_debug_console();
};
