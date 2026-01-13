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

#define UUID_t uint64_t


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

UUID_t generate_uuid();
enum class RenderCommandType {
    None = 0,
    Entity = 1,
    ImGuiWindow = 2
};

struct RenderCommand {
    RenderCommandType type = RenderCommandType::None;
    std::function<void()> execute_func;
    
    // Draw Context
    std::string shader_name = "None";
    std::string updater_name = "None";
    // Uniforms / State
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::mat4 transform = glm::mat4(1.0f);
    
    // Debugging
    std::string debug_name = "None";

};



// Forward declarations
class Orchestrator;
class Renderer;
class InputSystem;
class EntityManager;
class AggregateManager;
class WorldUpdater;
class ImGuiUpdater;
class ModeUpdater;
class SystemState;
// class WindowState;
class RenderCommandBuffer;
class Shader;
class ShaderManager;

struct CommandBundle {
    std::shared_ptr<Shader> shader;
    RenderCommand command;
    Renderer* renderer;
};