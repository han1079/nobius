#pragma once
#include <pch.h>
#include <core/common.h>
#include <utils/timestamped_state.h>

class InputSystem {
public:
    InputSystem();
    ~InputSystem() = default;
public:
    // UI Layout tracking
    TimeStampedFloat sidebar_x, sidebar_y, sidebar_width, sidebar_height;
    TimeStampedFloat bottom_x, bottom_y, bottom_width, bottom_height;
    TimeStampedFloat ribbon_x, ribbon_y, ribbon_width, ribbon_height;
    TimeStampedFloat viewport_x, viewport_y, viewport_width, viewport_height;
    // Input state
    TimeStampedBool ctrl_state, shift_state, alt_state;
    TimeStampedBool mouse_left_state, mouse_right_state, mouse_middle_state;
    TimeStampedFloat mouse_wheel_dy;
    TimeStampedFloat mouse_x, mouse_y;
    TimeStampedFloat mouse_x_relative, mouse_y_relative;
    TimeStampedBool key_states[SDL_NUM_SCANCODES];

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
    glm::vec2 get_mouse_position() const { return {mouse_x.value, mouse_y.value}; }
    glm::vec2 get_mouse_relative_position() const { return {mouse_x_relative.value, mouse_y_relative.value}; }
    glm::vec2 get_mouse_delta_position() const { return {mouse_x.delta_value, mouse_y.delta_value}; }
    bool is_key_pressed(SDL_Scancode key) const;
    bool just_pressed(SDL_Scancode key) const;
    bool is_ctrl_pressed() const { return ctrl_state.value; }
    bool is_shift_pressed() const { return shift_state.value; }
    bool is_alt_pressed() const { return alt_state.value; }
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
