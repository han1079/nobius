#pragma once
#include <pch.h>
#include <core/common.h>
#include <utils/timestamped_state.h>

// A snapshot of a window's layout in screen coordinates (Top-Left Origin)
// Components: x, y, width, height
using WindowRect = TimeStampedValue<glm::vec4>;

struct WindowLayout {
    WindowRect sidebar;
    WindowRect bottom_panel;
    WindowRect ribbon;
    WindowRect viewport;
};

struct MouseState {
    // Raw Screen Coordinates (Origin: Top-Left, Y-Down)
    // Used for: ImGui, SDL events, UI interaction
    TimeStampedValue<glm::vec2> screen_pos;

    // Cartesian Coordinates (Origin: Bottom-Left, Y-Up)
    // Used for: World rendering, OpenGL, Math
    // Calculated as: (screen_pos.x, window_height - screen_pos.y)
    TimeStampedValue<glm::vec2> cartesian_pos;

    // Relative Coordinates (Origin: Top-Left of the HOVERED element)
    // Useful for local UI interaction
    TimeStampedValue<glm::vec2> relative_pos;

    TimeStampedValue<float> wheel_delta;
    
    TimeStampedValue<bool> left_button;
    TimeStampedValue<bool> right_button;
    TimeStampedValue<bool> middle_button;

    // Context awareness
    HoveredUIElement hovered_element = HoveredUIElement::NONE;
    std::string hovered_element_name = "NONE";
};

struct KeyboardState {
    TimeStampedValue<bool> ctrl;
    TimeStampedValue<bool> shift;
    TimeStampedValue<bool> alt;
    TimeStampedValue<bool> keys[SDL_NUM_SCANCODES];
};

class InputSystem {
public:
    InputSystem();
    ~InputSystem() = default;
public:
    // State Structures
    WindowLayout windows;
    MouseState mouse;
    KeyboardState keyboard;

private:

    HoveredUIElement hovered_element = HoveredUIElement::NONE;
    std::string hovered_element_name = "NONE";
    float latest_time = 0.0f;
    bool space_just_true = false;


    // Mode state (consolidated from ModeState)
    uint32_t user_mode = (uint32_t)UserMode::MODE_SELECT;

    void update_mouse(float new_mouse_x, float new_mouse_y, float timestamp);

public:
    // Event processing
    void update_time(float timestamp);
    void update_window_hover();
    EngineEvent ingest_event(SDL_Event& event);
    void update_window_params(float& win_x, float& win_y, float& win_width, float& win_height, std::string window_name);
    void update_mode();
    
    // Frame management
    void save_accumulated_changes();
    void begin_frame(float timestamp);

    // Input state queries
    glm::vec2 get_mouse_position() const { return mouse.screen_pos.value; }
    glm::vec2 get_mouse_relative_position() const { return mouse.relative_pos.value; }
    glm::vec2 get_mouse_delta_position() const { return mouse.screen_pos.delta_value; }
    bool is_key_pressed(SDL_Scancode key) const;
    bool just_pressed(SDL_Scancode key) const;
    bool is_ctrl_pressed() const { return keyboard.ctrl.value; }
    bool is_shift_pressed() const { return keyboard.shift.value; }
    bool is_alt_pressed() const { return keyboard.alt.value; }
    bool is_mouse_pressed(int button) const;
    
    // Mode management
    bool is_mode(UserMode mode) const { return (user_mode & (uint32_t)mode) != 0; }
    void set_mode(UserMode mode) { user_mode |= (uint32_t)mode; }
    void clear_mode(UserMode mode) { user_mode &= ~(uint32_t)mode; }
    void toggle_mode(UserMode mode) { user_mode ^= (uint32_t)mode; }
    uint32_t get_mode_flags() const { return user_mode; }
    void set_closed() { user_mode = (uint32_t)UserMode::MODE_CLOSE_REQUESTED; }
    
    // UI element queries
    HoveredUIElement get_hovered_element() const { return hovered_element; }
    const std::string& get_hovered_element_name() const { return hovered_element_name; }
};
