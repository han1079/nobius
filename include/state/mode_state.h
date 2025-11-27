#pragma once

#include <core/base_state.h>
#include <core/common.h>
#include <pch.h>
#include <imgui_internal.h>
// enum USER_MODES {
//     MODE_SELECT = (1 << 0), // Select Mode. Objects hover, and can be clicked on
//     MODE_DRAG = (1 << 1), // Drag Mode. Entity is currently being selected and edited
//     MODE_MOVE = (1 << 2), // Move Mode. Entity is being moved via center.
//     MODE_PLACE = (1 << 3), // Place Mode. New Entity is being placed. Preview is shown.
//     MODE_DELETE = (1 << 4), // Delete Mode. Entities can be clicked to delete.
//     MODE_INACTIVE = (1 << 5), // No Viewport Interaction Logic apart from "Reactivate"
//     MODE_UIFOCUS = (1 << 6), // UI is being interacted with - runs off ImGui IO flag
//     MODE_CLOSE_REQUESTED = 0 // Application close has been requested
// };

struct TimeStampedBoolEvent {
    bool value = false;
    int changes_since_poll = 0;
    bool last_value = false;
    float timestamp = 0.0f;

    inline void update(bool new_value, float new_time) {
        if (new_value != value) {
            last_value = value;
            value = new_value;
            changes_since_poll += 1;
            timestamp = new_time;
        }
    }

    inline void reset_changes() {
        changes_since_poll = 0;
        last_value = value;
    }
};

struct TimeStampedFloatEvent {
    float value = 0.0f;
    int changes_since_poll = 0;
    float last_value = 0.0f;
    float timestamp = 0.0f;

    inline void update(float new_value, float new_time) {
        if (new_value != value) {
            last_value = value;
            value = new_value;
            changes_since_poll += 1;
            timestamp = new_time;
        }
    }

    inline void reset_changes() {
        changes_since_poll = 0;
        last_value = value;
    }
};



class UIState : public BaseState {
public:
    UIState() {
        DEBUG_HOOK_FUNCTION_NO_TIMER();
        DEBUG_HOOK_VAR_AS(hovered_element_name, "UI_HOVERED_ELEMENT");
        DEBUG_HOOK_VAR_AS(mouse_x.value, "UI_MOUSE_X");
        DEBUG_HOOK_VAR_AS(mouse_y.value, "UI_MOUSE_Y");
        DEBUG_HOOK_VAR_AS(mouse_x_relative.value, "UI_MOUSE_X_RELATIVE");
        DEBUG_HOOK_VAR_AS(mouse_y_relative.value, "UI_MOUSE_Y_RELATIVE");
    }
    ~UIState() = default;

    template <typename member_type>
    auto get(member_type member_name) -> decltype(this->*member_name) {
        return (this)->*member_name;
    }

    template <typename member_type, typename value_type>
    void set(member_type member_name, value_type val) {
        (this)->*member_name = val; 
    }

private:


    TimeStampedFloatEvent sidebar_x;
    TimeStampedFloatEvent sidebar_y;
    TimeStampedFloatEvent sidebar_width;
    TimeStampedFloatEvent sidebar_height;

    TimeStampedFloatEvent bottom_x;
    TimeStampedFloatEvent bottom_y;
    TimeStampedFloatEvent bottom_width;
    TimeStampedFloatEvent bottom_height;

    TimeStampedFloatEvent ribbon_x;
    TimeStampedFloatEvent ribbon_y;
    TimeStampedFloatEvent ribbon_width;
    TimeStampedFloatEvent ribbon_height;

    TimeStampedFloatEvent viewport_x;
    TimeStampedFloatEvent viewport_y;
    TimeStampedFloatEvent viewport_width;
    TimeStampedFloatEvent viewport_height;

    HoveredUIElement hovered_element = HoveredUIElement::NONE;
    std::string hovered_element_name = "NONE";
    float latest_time = 0.0f;

    TimeStampedBoolEvent ctrl_state;
    TimeStampedBoolEvent shift_state;
    TimeStampedBoolEvent alt_state;
    TimeStampedBoolEvent mouse_left_state;
    TimeStampedBoolEvent mouse_right_state;
    TimeStampedBoolEvent mouse_middle_state;
    
    TimeStampedFloatEvent mouse_wheel_dy;
    TimeStampedFloatEvent mouse_x;
    TimeStampedFloatEvent mouse_y;

    TimeStampedFloatEvent mouse_x_relative;
    TimeStampedFloatEvent mouse_y_relative;

    TimeStampedBoolEvent KeyStates[SDL_NUM_SCANCODES];

    friend class EventIngester;

    
};

class ModeState : public BaseState { 
public:
    ModeState() = default;
    ModeState(const std::optional<std::string>& fpath);
    ~ModeState() = default;

protected:
    void load_from_json(const std::optional<std::string>& fpath = std::nullopt) override;

public:
    // Mode Information
    
    uint32_t user_mode = (uint32_t)UserMode::MODE_SELECT;
    template <typename member_type>
    auto get(member_type member_name) -> decltype(this->*member_name) {
        return (this)->*member_name;
    }

    template <typename member_type, typename value_type>
    void set(member_type member_name, value_type val) {
        (this)->*member_name = val; 
    }

    uint32_t check_flag(uint32_t flag) {
        return (user_mode & flag);
    }

    uint32_t set_flag(uint32_t flag) {
        user_mode = (user_mode | flag);
        return user_mode;
    }

    uint32_t clear_flag(uint32_t flag) {
        user_mode = (user_mode & (~flag));
        return user_mode;
    }

    void set_closed() {
        user_mode = (uint32_t)UserMode::MODE_CLOSE_REQUESTED; //Zeros everything out - invalidates all other flags.
    }
};
