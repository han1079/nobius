
#include <state/renderer_state.h>

RendererState::RendererState(const std::optional<std::string>& fpath) : BaseState(fpath) {
    load_from_json(fpath);
} 

// Destructor implementation
RendererState::~RendererState() = default;

void RendererState::save_imgui_style(const std::string& fpath) {
    if (fpath.empty()) {
        return;
    }

    nlohmann::json j;
    ImGuiStyle& style = ImGui::GetStyle();

    // Style sizes and spacing
    j["window_padding"] = {style.WindowPadding.x, style.WindowPadding.y};
    j["window_rounding"] = style.WindowRounding;
    j["window_border_size"] = style.WindowBorderSize;
    j["window_min_size"] = {style.WindowMinSize.x, style.WindowMinSize.y};
    j["window_title_align"] = {style.WindowTitleAlign.x, style.WindowTitleAlign.y};
    j["window_menu_button_position"] = static_cast<int>(style.WindowMenuButtonPosition);
    
    j["child_rounding"] = style.ChildRounding;
    j["child_border_size"] = style.ChildBorderSize;
    
    j["popup_rounding"] = style.PopupRounding;
    j["popup_border_size"] = style.PopupBorderSize;
    
    j["frame_padding"] = {style.FramePadding.x, style.FramePadding.y};
    j["frame_rounding"] = style.FrameRounding;
    j["frame_border_size"] = style.FrameBorderSize;
    
    j["item_spacing"] = {style.ItemSpacing.x, style.ItemSpacing.y};
    j["item_inner_spacing"] = {style.ItemInnerSpacing.x, style.ItemInnerSpacing.y};
    j["cell_padding"] = {style.CellPadding.x, style.CellPadding.y};
    
    j["touch_extra_padding"] = {style.TouchExtraPadding.x, style.TouchExtraPadding.y};
    j["indent_spacing"] = style.IndentSpacing;
    j["columns_min_spacing"] = style.ColumnsMinSpacing;
    j["scrollbar_size"] = style.ScrollbarSize;
    j["scrollbar_rounding"] = style.ScrollbarRounding;
    j["grab_min_size"] = style.GrabMinSize;
    j["grab_rounding"] = style.GrabRounding;
    j["log_slider_deadzone"] = style.LogSliderDeadzone;
    j["tab_rounding"] = style.TabRounding;
    j["tab_border_size"] = style.TabBorderSize;
    j["color_button_position"] = static_cast<int>(style.ColorButtonPosition);
    j["button_text_align"] = {style.ButtonTextAlign.x, style.ButtonTextAlign.y};
    j["selectable_text_align"] = {style.SelectableTextAlign.x, style.SelectableTextAlign.y};
    
    j["display_window_padding"] = {style.DisplayWindowPadding.x, style.DisplayWindowPadding.y};
    j["display_safe_area_padding"] = {style.DisplaySafeAreaPadding.x, style.DisplaySafeAreaPadding.y};
    j["mouse_cursor_scale"] = style.MouseCursorScale;
    j["anti_aliased_lines"] = style.AntiAliasedLines;
    j["anti_aliased_lines_use_tex"] = style.AntiAliasedLinesUseTex;
    j["anti_aliased_fill"] = style.AntiAliasedFill;
    j["curve_tessellation_tol"] = style.CurveTessellationTol;
    j["circle_tessellation_max_error"] = style.CircleTessellationMaxError;

    // Colors (all 53 colors in ImGuiCol enum)
    nlohmann::json colors = nlohmann::json::array();
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
        colors.push_back({
            style.Colors[i].x,
            style.Colors[i].y, 
            style.Colors[i].z,
            style.Colors[i].w
        });
    }
    j["colors"] = colors;

    // Write to file
    std::ofstream file(fpath);
    file << j.dump(4); // Pretty print with 4 spaces
    file.close();
}

void RendererState::load_from_json(const std::optional<std::string>& fpath) {
    if (!fpath || fpath->empty()) {
        return;
    }

    std::ifstream file(*fpath);
    nlohmann::json j = nlohmann::json::parse(file);

    // Load SDL configuration
    load_sdl_config_from_json(j);

    // Load ImGui style if present
    if (j.contains("imgui_style_data")) {
        load_imgui_style_from_json(j["imgui_style_data"]);
    }
}

void RendererState::load_sdl_config_from_json(const nlohmann::json& sdl_json) {
    // Check for compatibility/version if needed
    if (sdl_json.contains("config_version")) {
        // You can add version checking logic here
        // e.g., if (sdl_json["config_version"] < minimum_version) return;
    }

    if (sdl_json.contains("sdl_flags")) {
        sdl_flags = static_cast<int>(sdl_json["sdl_flags"]);
    }
    
    if (sdl_json.contains("sdl_profile_mask")) {
        if (sdl_json["sdl_profile_mask"] == "core") {
            sdl_profile_mask = SDL_GL_CONTEXT_PROFILE_CORE;
        }
        // Add other profile options as needed
    }

    if (sdl_json.contains("sdl_major_version")) {
        sdl_major_version = sdl_json["sdl_major_version"].get<int>();
    }
    if (sdl_json.contains("sdl_minor_version")) {
        sdl_minor_version = sdl_json["sdl_minor_version"].get<int>();
    }
    if (sdl_json.contains("sdl_double_buffer")) {
        sdl_double_buffer = sdl_json["sdl_double_buffer"].get<int>();
    }

    if (sdl_json.contains("sdl_depth_size")) {
        sdl_depth_size = sdl_json["sdl_depth_size"].get<int>();
    }
    if (sdl_json.contains("sdl_stencil_size")) {
        sdl_stencil_size = sdl_json["sdl_stencil_size"].get<int>();
    }
    
    if (sdl_json.contains("sdl_window_flags")) {
        sdl_window_flags = (SDL_WindowFlags)(0);
        for (const std::string& val : sdl_json["sdl_window_flags"]) {
            sdl_window_flags = (SDL_WindowFlags)((uint32_t)sdl_window_flags | 
                                                 (uint32_t)flag_from_string(sdl_window_flag_lookup, val));
        }
    }

    if (sdl_json.contains("sdl_window_width")) {
        sdl_window_width = sdl_json["sdl_window_width"].get<int>();
    }
    if (sdl_json.contains("sdl_window_height")) {
        sdl_window_height = sdl_json["sdl_window_height"].get<int>();
    }
    if (sdl_json.contains("sdl_window_title")) {
        sdl_window_title = sdl_json["sdl_window_title"].get<std::string>();
    }

    if (sdl_json.contains("glsl_version")) {
        glsl_version = sdl_json["glsl_version"].get<std::string>();
    }
    if (sdl_json.contains("imgui_style")) {
        imgui_style = sdl_json["imgui_style"].get<std::string>();
    }

    if (sdl_json.contains("gl_clear_color")) {
        auto color_array = sdl_json["gl_clear_color"];
        gl_clear_color = glm::vec4(color_array[0].get<float>(),
                                   color_array[1].get<float>(),
                                   color_array[2].get<float>(),
                                   color_array[3].get<float>());
    }
}

void RendererState::load_imgui_style_from_json(const nlohmann::json& style_json) {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Load style sizes and spacing
    if (style_json.contains("window_padding")) {
        auto wp = style_json["window_padding"];
        style.WindowPadding = ImVec2(wp[0], wp[1]);
    }
    if (style_json.contains("window_rounding")) {
        style.WindowRounding = style_json["window_rounding"];
    }
    if (style_json.contains("window_border_size")) {
        style.WindowBorderSize = style_json["window_border_size"];
    }
    if (style_json.contains("window_min_size")) {
        auto wms = style_json["window_min_size"];
        style.WindowMinSize = ImVec2(wms[0], wms[1]);
    }
    if (style_json.contains("window_title_align")) {
        auto wta = style_json["window_title_align"];
        style.WindowTitleAlign = ImVec2(wta[0], wta[1]);
    }
    if (style_json.contains("window_menu_button_position")) {
        style.WindowMenuButtonPosition = static_cast<ImGuiDir>(style_json["window_menu_button_position"]);
    }
    
    if (style_json.contains("child_rounding")) {
        style.ChildRounding = style_json["child_rounding"];
    }
    if (style_json.contains("child_border_size")) {
        style.ChildBorderSize = style_json["child_border_size"];
    }
    
    if (style_json.contains("popup_rounding")) {
        style.PopupRounding = style_json["popup_rounding"];
    }
    if (style_json.contains("popup_border_size")) {
        style.PopupBorderSize = style_json["popup_border_size"];
    }
    
    if (style_json.contains("frame_padding")) {
        auto fp = style_json["frame_padding"];
        style.FramePadding = ImVec2(fp[0], fp[1]);
    }
    if (style_json.contains("frame_rounding")) {
        style.FrameRounding = style_json["frame_rounding"];
    }
    if (style_json.contains("frame_border_size")) {
        style.FrameBorderSize = style_json["frame_border_size"];
    }
    
    if (style_json.contains("item_spacing")) {
        auto is = style_json["item_spacing"];
        style.ItemSpacing = ImVec2(is[0], is[1]);
    }
    if (style_json.contains("item_inner_spacing")) {
        auto iis = style_json["item_inner_spacing"];
        style.ItemInnerSpacing = ImVec2(iis[0], iis[1]);
    }
    if (style_json.contains("cell_padding")) {
        auto cp = style_json["cell_padding"];
        style.CellPadding = ImVec2(cp[0], cp[1]);
    }
    
    if (style_json.contains("touch_extra_padding")) {
        auto tep = style_json["touch_extra_padding"];
        style.TouchExtraPadding = ImVec2(tep[0], tep[1]);
    }
    if (style_json.contains("indent_spacing")) {
        style.IndentSpacing = style_json["indent_spacing"];
    }
    if (style_json.contains("columns_min_spacing")) {
        style.ColumnsMinSpacing = style_json["columns_min_spacing"];
    }
    if (style_json.contains("scrollbar_size")) {
        style.ScrollbarSize = style_json["scrollbar_size"];
    }
    if (style_json.contains("scrollbar_rounding")) {
        style.ScrollbarRounding = style_json["scrollbar_rounding"];
    }
    if (style_json.contains("grab_min_size")) {
        style.GrabMinSize = style_json["grab_min_size"];
    }
    if (style_json.contains("grab_rounding")) {
        style.GrabRounding = style_json["grab_rounding"];
    }
    if (style_json.contains("log_slider_deadzone")) {
        style.LogSliderDeadzone = style_json["log_slider_deadzone"];
    }
    if (style_json.contains("tab_rounding")) {
        style.TabRounding = style_json["tab_rounding"];
    }
    if (style_json.contains("tab_border_size")) {
        style.TabBorderSize = style_json["tab_border_size"];
    }
    if (style_json.contains("color_button_position")) {
        style.ColorButtonPosition = static_cast<ImGuiDir>(style_json["color_button_position"]);
    }
    if (style_json.contains("button_text_align")) {
        auto bta = style_json["button_text_align"];
        style.ButtonTextAlign = ImVec2(bta[0], bta[1]);
    }
    if (style_json.contains("selectable_text_align")) {
        auto sta = style_json["selectable_text_align"];
        style.SelectableTextAlign = ImVec2(sta[0], sta[1]);
    }
    
    if (style_json.contains("display_window_padding")) {
        auto dwp = style_json["display_window_padding"];
        style.DisplayWindowPadding = ImVec2(dwp[0], dwp[1]);
    }
    if (style_json.contains("display_safe_area_padding")) {
        auto dsap = style_json["display_safe_area_padding"];
        style.DisplaySafeAreaPadding = ImVec2(dsap[0], dsap[1]);
    }
    if (style_json.contains("mouse_cursor_scale")) {
        style.MouseCursorScale = style_json["mouse_cursor_scale"];
    }
    if (style_json.contains("anti_aliased_lines")) {
        style.AntiAliasedLines = style_json["anti_aliased_lines"];
    }
    if (style_json.contains("anti_aliased_lines_use_tex")) {
        style.AntiAliasedLinesUseTex = style_json["anti_aliased_lines_use_tex"];
    }
    if (style_json.contains("anti_aliased_fill")) {
        style.AntiAliasedFill = style_json["anti_aliased_fill"];
    }
    if (style_json.contains("curve_tessellation_tol")) {
        style.CurveTessellationTol = style_json["curve_tessellation_tol"];
    }
    if (style_json.contains("circle_tessellation_max_error")) {
        style.CircleTessellationMaxError = style_json["circle_tessellation_max_error"];
    }

    // Load colors
    if (style_json.contains("colors")) {
        auto colors = style_json["colors"];
        for (int i = 0; i < ImGuiCol_COUNT && i < colors.size(); i++) {
            auto color = colors[i];
            style.Colors[i] = ImVec4(color[0], color[1], color[2], color[3]);
        }
    }


