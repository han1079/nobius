#pragma once

#include <pch.h>



/*  CMake should give the exact path to source, but this
    acts as a stopgap in weird cases / for LSPs to no freak out.
    build/ should be one stop above source regardless in my structure.
 */
#ifndef PROJECT_SOURCE_DIR
#define PROJECT_SOURCE_DIR "../"
#endif


#define ORCH() Orchestrator::get()
#define ENUM_NAME(x) {x, #x}


// Grow this list of events to include events that I actually care 
// about from the perspective of non-ImGui state update interactions 
// This is mostly user input stuff that draws stuff, like mouse 
// movements and keyboard inputs. I will ignore all sorts 
// of window stuff here.
enum class EngineEventType {
    None,
    Captured,
    Quit,
    NoScreen,
    MouseMove,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel,
    KeyDown,
    KeyUp,
};

inline static std::unordered_map<EngineEventType, std::string> EngineEventTypeNames = {
    ENUM_NAME(EngineEventType::None),
    ENUM_NAME(EngineEventType::Captured),
    ENUM_NAME(EngineEventType::Quit),
    ENUM_NAME(EngineEventType::NoScreen),
    ENUM_NAME(EngineEventType::MouseMove),
    ENUM_NAME(EngineEventType::MouseButtonDown),
    ENUM_NAME(EngineEventType::MouseButtonUp),
    ENUM_NAME(EngineEventType::MouseWheel),
    ENUM_NAME(EngineEventType::KeyDown),
    ENUM_NAME(EngineEventType::KeyUp),
};

struct EngineEvent {
    EngineEventType type = EngineEventType::None;

    int mouse_x = 0;
    int mouse_y = 0;
    int mouse_button = 0;
    int wheel_dy = 0;
    bool wheel_direction_up = false;
    
    SDL_Keysym key_info = {};
};

enum class HoveredUIElement {
    NONE,
    SIDEBAR,
    BOTTOM_PANEL,
    RIBBON,
    VIEWPORT
};

inline static std::unordered_map<HoveredUIElement, std::string> HoveredUIElementNames = {
    ENUM_NAME(HoveredUIElement::NONE),
    ENUM_NAME(HoveredUIElement::SIDEBAR),
    ENUM_NAME(HoveredUIElement::BOTTOM_PANEL),
    ENUM_NAME(HoveredUIElement::RIBBON),
    ENUM_NAME(HoveredUIElement::VIEWPORT),
};
enum class UserMode {
    MODE_SELECT = (1 << 0), // Select Mode. Objects hover, and can be clicked on
    MODE_DRAG = (1 << 1), // Drag Mode. Entity is currently being selected and edited
    MODE_MOVE = (1 << 2), // Move Mode. Entity is being moved via center.
    MODE_PLACE = (1 << 3), // Place Mode. New Entity is being placed. Preview is shown.
    MODE_DELETE = (1 << 4), // Delete Mode. Entities can be clicked to delete.
    MODE_INACTIVE = (1 << 5), // No Viewport Interaction Logic apart from "Reactivate"
    MODE_UIFOCUS = (1 << 6), // UI is being interacted with - runs off ImGui IO flag
    MODE_CLOSE_REQUESTED = 0 // Application close has been requested
};

inline static std::unordered_map<UserMode, std::string> UserModeNames = {
    ENUM_NAME(UserMode::MODE_SELECT),
    ENUM_NAME(UserMode::MODE_DRAG),
    ENUM_NAME(UserMode::MODE_MOVE),
    ENUM_NAME(UserMode::MODE_PLACE),
    ENUM_NAME(UserMode::MODE_DELETE),
    ENUM_NAME(UserMode::MODE_INACTIVE),
    ENUM_NAME(UserMode::MODE_UIFOCUS),
    ENUM_NAME(UserMode::MODE_CLOSE_REQUESTED),
};

struct LoadSpec {
    std::optional<std::string> world_state_spec = std::nullopt;
    std::optional<std::string> mode_state_spec = std::nullopt;
    std::optional<std::string> imgui_state_spec = std::nullopt;
};

inline const std::unordered_map<std::string, SDL_WindowFlags> sdl_window_flag_lookup = {
    {"fullscreen",             SDL_WINDOW_FULLSCREEN},
    {"opengl",                 SDL_WINDOW_OPENGL},
    {"shown",                  SDL_WINDOW_SHOWN},
    {"hidden",                 SDL_WINDOW_HIDDEN},
    {"borderless",             SDL_WINDOW_BORDERLESS},
    {"resizable",              SDL_WINDOW_RESIZABLE},
    {"minimized",              SDL_WINDOW_MINIMIZED},
    {"maximized",              SDL_WINDOW_MAXIMIZED},
    {"mouse_grabbed",          SDL_WINDOW_MOUSE_GRABBED},
    {"input_focus",            SDL_WINDOW_INPUT_FOCUS},
    {"mouse_focus",            SDL_WINDOW_MOUSE_FOCUS},
    {"fullscreen_desktop",     SDL_WINDOW_FULLSCREEN_DESKTOP},
    {"foreign",                SDL_WINDOW_FOREIGN},
    {"allow_highdpi",          SDL_WINDOW_ALLOW_HIGHDPI},
    {"mouse_capture",          SDL_WINDOW_MOUSE_CAPTURE},
    {"always_on_top",          SDL_WINDOW_ALWAYS_ON_TOP},
    {"skip_taskbar",           SDL_WINDOW_SKIP_TASKBAR},
    {"utility",                SDL_WINDOW_UTILITY},
    {"tooltip",                SDL_WINDOW_TOOLTIP},
    {"popup_menu",             SDL_WINDOW_POPUP_MENU},
    {"keyboard_grabbed",       SDL_WINDOW_KEYBOARD_GRABBED},
    {"vulkan",                 SDL_WINDOW_VULKAN},
    {"metal",                  SDL_WINDOW_METAL},
    {"input_grabbed",          SDL_WINDOW_INPUT_GRABBED}
};


inline const std::unordered_map<std::string, SDL_WindowEventID> sdl_window_event_id_lookup = {
    {"none",              SDL_WINDOWEVENT_NONE},
    {"shown",             SDL_WINDOWEVENT_SHOWN},
    {"hidden",            SDL_WINDOWEVENT_HIDDEN},
    {"exposed",           SDL_WINDOWEVENT_EXPOSED},
    {"moved",             SDL_WINDOWEVENT_MOVED},
    {"resized",           SDL_WINDOWEVENT_RESIZED},
    {"size_changed",      SDL_WINDOWEVENT_SIZE_CHANGED},
    {"minimized",         SDL_WINDOWEVENT_MINIMIZED},
    {"maximized",         SDL_WINDOWEVENT_MAXIMIZED},
    {"restored",          SDL_WINDOWEVENT_RESTORED},
    {"enter",             SDL_WINDOWEVENT_ENTER},
    {"leave",             SDL_WINDOWEVENT_LEAVE},
    {"focus_gained",      SDL_WINDOWEVENT_FOCUS_GAINED},
    {"focus_lost",        SDL_WINDOWEVENT_FOCUS_LOST},
    {"close",             SDL_WINDOWEVENT_CLOSE},
    {"take_focus",        SDL_WINDOWEVENT_TAKE_FOCUS},
    {"hit_test",          SDL_WINDOWEVENT_HIT_TEST},
    {"iccprof_changed",   SDL_WINDOWEVENT_ICCPROF_CHANGED},
    {"display_changed",   SDL_WINDOWEVENT_DISPLAY_CHANGED}
};

inline const std::unordered_map<std::string, SDL_GLattr> sdl_gl_attr_lookup = {
    {"red_size",                     SDL_GL_RED_SIZE},
    {"green_size",                   SDL_GL_GREEN_SIZE},
    {"blue_size",                    SDL_GL_BLUE_SIZE},
    {"alpha_size",                   SDL_GL_ALPHA_SIZE},
    {"buffer_size",                  SDL_GL_BUFFER_SIZE},
    {"doublebuffer",                 SDL_GL_DOUBLEBUFFER},
    {"depth_size",                   SDL_GL_DEPTH_SIZE},
    {"stencil_size",                 SDL_GL_STENCIL_SIZE},
    {"accum_red_size",               SDL_GL_ACCUM_RED_SIZE},
    {"accum_green_size",             SDL_GL_ACCUM_GREEN_SIZE},
    {"accum_blue_size",              SDL_GL_ACCUM_BLUE_SIZE},
    {"accum_alpha_size",             SDL_GL_ACCUM_ALPHA_SIZE},
    {"stereo",                       SDL_GL_STEREO},
    {"multisamplebuffers",           SDL_GL_MULTISAMPLEBUFFERS},
    {"multisamplesamples",           SDL_GL_MULTISAMPLESAMPLES},
    {"accelerated_visual",           SDL_GL_ACCELERATED_VISUAL},
    {"retained_backing",             SDL_GL_RETAINED_BACKING},
    {"context_major_version",        SDL_GL_CONTEXT_MAJOR_VERSION},
    {"context_minor_version",        SDL_GL_CONTEXT_MINOR_VERSION},
    {"context_egl",                  SDL_GL_CONTEXT_EGL},
    {"context_flags",                SDL_GL_CONTEXT_FLAGS},
    {"context_profile_mask",         SDL_GL_CONTEXT_PROFILE_MASK},
    {"share_with_current_context",   SDL_GL_SHARE_WITH_CURRENT_CONTEXT},
    {"framebuffer_srgb_capable",     SDL_GL_FRAMEBUFFER_SRGB_CAPABLE},
    {"context_release_behavior",     SDL_GL_CONTEXT_RELEASE_BEHAVIOR},
    {"context_reset_notification",   SDL_GL_CONTEXT_RESET_NOTIFICATION},
    {"context_no_error",             SDL_GL_CONTEXT_NO_ERROR},
    {"floatbuffers",                 SDL_GL_FLOATBUFFERS}
};

inline const std::unordered_map<std::string, SDL_GLprofile> sdl_gl_profile_lookup = {
    {"core",          SDL_GL_CONTEXT_PROFILE_CORE},
    {"compatibility", SDL_GL_CONTEXT_PROFILE_COMPATIBILITY},
    {"es",            SDL_GL_CONTEXT_PROFILE_ES}
};
template <typename flag_type>
inline flag_type flag_from_string(const std::unordered_map<std::string, flag_type>& lookup,
                                  const std::string& key,
                                  flag_type default_val = static_cast<flag_type>(0)) noexcept 
{
    if (auto it = lookup.find(key); it != lookup.end()){
        return it->second;
    }
    return default_val;
}

template <typename flag_type>
inline std::string string_from_flag(const std::unordered_map<std::string, flag_type>& lookup,
                                  const flag_type& flag_value,
                                  std::string default_val = "unknown") noexcept
{
    // Simple linear search - fine for small lookup tables
    for (const auto& [str_key, flag_val]: lookup) {
        if (flag_val == flag_value) {
            return str_key;
        }
    }
    return default_val;
}
// Forward declarations
class Orchestrator;
class Renderer;
class WorldUpdater;
class InputSystem;
class ImGuiUpdater;
class ModeUpdater;
class SystemState;
class WindowState;
class RenderCommandBuffer;
