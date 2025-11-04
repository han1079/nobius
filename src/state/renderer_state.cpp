
#include <state/renderer_state.h>

RendererState::RendererState(const std::optional<std::string>& fpath) : BaseState(fpath) {
    load_from_json(fpath);
} 

// Destructor implementation
RendererState::~RendererState() = default;

void RendererState::load_from_json(const std::optional<std::string>& fpath) {
    if (!fpath || fpath->empty()) {
        return;
    }

    std::ifstream file(*fpath);
    nlohmann::json j = nlohmann::json::parse(file);

    sdl_flags = static_cast<int>(j["sdl_flags"]);
    
    if (j["sdl_profile_mask"] == "core") {
        sdl_profile_mask = SDL_GL_CONTEXT_PROFILE_CORE;
    }

    sdl_major_version = j["sdl_major_version"].get<int>();
    sdl_minor_version = j["sdl_minor_version"].get<int>();
    sdl_double_buffer = j["sdl_double_buffer"].get<int>();

    sdl_depth_size = j["sdl_depth_size"].get<int>();
    sdl_stencil_size = j["sdl_stencil_size"].get<int>();
    
    sdl_window_flags = (SDL_WindowFlags)(0);
    for (const std::string& val : j["sdl_window_flags"]) {
        sdl_window_flags = (SDL_WindowFlags)((uint32_t)sdl_window_flags | 
                                             (uint32_t)flag_from_string(sdl_window_flag_lookup, val));
    }

    sdl_window_width = j["sdl_window_width"].get<int>();
    sdl_window_height = j["sdl_window_height"].get<int>();
    sdl_window_title = j["sdl_window_title"].get<std::string>();

    glsl_version = j["glsl_version"].get<std::string>();
    imgui_style = j["imgui_style"].get<std::string>();

    auto color_array = j["gl_clear_color"];

    gl_clear_color = glm::vec4(color_array[0].get<float>(),
                               color_array[1].get<float>(),
                               color_array[2].get<float>(),
                               color_array[3].get<float>());

}


