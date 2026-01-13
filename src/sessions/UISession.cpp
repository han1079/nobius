#include <sessions/UISession.h>
#include <utils/file_utils.h>

void UISession::on_enter() {
    Renderer& r = renderer();
    UI::initialize_imgui(r.sdl_window, r.gl_context, r.scale);
    
    // Load window configurations
    build_windows();
}

void UISession::on_exit() {
    // TODO: Cleanup UI session resources
}

void UISession::process_input_deltas(DispatchHistory& h) {
    UI::update_imgui_state();
}

void UISession::process_time_deltas(float dT) {
    // TODO: Update UI animations, tweens, etc.
    return;
}

void UISession::submit_render_request() {
    Renderer& r = renderer();

    // Get window states (ensure build_windows() was called in on_enter)
    WindowState& sidebar_state = window_states.at("Bottom Panel");
    WindowState& bottom_panel_state = window_states.at("Bottom Panel");
    WindowState& ribbon_state = window_states.at("Bottom Panel");
    WindowState& main_window_state = window_states.at("Bottom Panel");
    WindowState& debug_console_state = window_states.at("Debug Console");
    
    // Create render commands with window state references
    RenderCommand build_sidebar = {
        RenderCommandType::ImGuiWindow, 
        [&sidebar_state]() { UI::build_sidebar(sidebar_state); }, 
        "IMGUI_INTERNAL", 
        "build_sidebar"
    };
    
    RenderCommand build_bottom_panel = {
        RenderCommandType::ImGuiWindow, 
        [&bottom_panel_state]() { UI::build_bottom_panel(bottom_panel_state); }, 
        "IMGUI_INTERNAL", 
        "build_bottom_panel"
    };
    
    RenderCommand build_ribbon = {
        RenderCommandType::ImGuiWindow, 
        [&ribbon_state]() { UI::build_ribbon(ribbon_state); }, 
        "IMGUI_INTERNAL", 
        "build_ribbon"
    };
    
    RenderCommand build_main_window = {
        RenderCommandType::ImGuiWindow, 
        [&main_window_state]() { UI::build_main_window(main_window_state); }, 
        "IMGUI_INTERNAL", 
        "build_main_window"
    };
    
    RenderCommand build_debug_console = {
        RenderCommandType::ImGuiWindow, 
        [&debug_console_state]() { UI::build_debug_console(debug_console_state); }, 
        "IMGUI_INTERNAL", 
        "build_debug_console"
    };

    // Submit all render commands
    r.submit_render_request(build_sidebar);
    r.submit_render_request(build_bottom_panel);
    r.submit_render_request(build_ribbon);
    r.submit_render_request(build_main_window);
}

void UISession::build_windows() {
    nlohmann::json to_build = FileUtils::read_file_to_json("configs/ui_config.json");
    
    for (auto& path : to_build["to_build"]) {
        std::string p = "configs/" + std::string(path);
        nlohmann::json build_config = FileUtils::read_file_to_json(p);
        
        // Extract position and size factors
        ImVec2 initial_pos_factor = {
            build_config["initial_pos_factor"][0].get<float>(),
            build_config["initial_pos_factor"][1].get<float>()
        };
        
        ImVec2 initial_size_factor = {
            build_config["initial_size_factor"][0].get<float>(),
            build_config["initial_size_factor"][1].get<float>()
        };
        
        WindowState state = {
            build_config["visible"].get<bool>(),
            build_config["window_name"].get<std::string>(),
            {0, 0},  // pos (calculated at runtime)
            {0, 0},  // size (calculated at runtime)
            initial_pos_factor,
            initial_size_factor,
            {}  // texts (populate below)
        };

        // Populate texts map if window_flags exists
        if (build_config.contains("window_flags") && build_config["window_flags"].is_object()) {
            for (auto& [key, value] : build_config["window_flags"].items()) {
                state.texts[key] = value.get<std::string>();
            }
        }
        
        window_states.try_emplace(state.window_name, state);
    }

    WindowState debug_console_state;
    debug_console_state.visible = true;
    debug_console_state.window_name = "Debug Console";
    window_states.try_emplace(debug_console_state.window_name, debug_console_state);
}
