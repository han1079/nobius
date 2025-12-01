#include <updaters/input_system.h>
#include <imgui_internal.h>

InputSystem::InputSystem() 
    : ctrl_state(), shift_state(), alt_state(),
      mouse_left_state(), mouse_right_state(), mouse_middle_state(),
      mouse_wheel_dy(),
      mouse_x(), mouse_y(),
      mouse_x_relative(), mouse_y_relative()
{
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        key_states[i] = TimeStampedBool();
    }
    DEBUG_HOOK_FUNCTION_NO_TIMER();
    DEBUG_HOOK_VAR_AS(hovered_element_name, "HOVERED_UI_ELEMENT_NAME");
    DEBUG_HOOK_VAR_AS(mouse_x.value, "MOUSE_X");
    DEBUG_HOOK_VAR_AS(mouse_y.value, "MOUSE_Y");
    DEBUG_HOOK_VAR_AS(mouse_x_relative.value, "MOUSE_X_RELATIVE");
    DEBUG_HOOK_VAR_AS(mouse_y_relative.value, "MOUSE_Y_RELATIVE");
    DEBUG_HOOK_VAR_AS(user_mode, "USER_MODE_FLAGS");
    DEBUG_HOOK_VAR_AS(key_states[SDL_SCANCODE_SPACE].value, "KEY_SPACE_STATE");
    DEBUG_HOOK_VAR_AS(space_just_true, "KEY_SPACE_JUST_TRUE");
}

void InputSystem::update_time(float timestamp) {
    latest_time = timestamp;
}

bool InputSystem::is_key_pressed(SDL_Scancode key) const {
    if (key >= 0 && key < SDL_NUM_SCANCODES) {
        return key_states[key].value;
    }
    return false;
}

bool InputSystem::just_pressed(SDL_Scancode key) const {
    if (key >= 0 && key < SDL_NUM_SCANCODES) {
        return key_states[key].just_became_true();
    }
    return false;
}

bool InputSystem::is_mouse_pressed(int button) const {
    switch(button) {
        case SDL_BUTTON_LEFT: return mouse_left_state.value;
        case SDL_BUTTON_RIGHT: return mouse_right_state.value;
        case SDL_BUTTON_MIDDLE: return mouse_middle_state.value;
        default: return false;
    }
}

void InputSystem::update_window_hover() {
    ImGuiWindow* hovered_window = ImGui::GetCurrentContext()->HoveredWindow;
    std::string window_name;

    
    if (hovered_window) {
        window_name = std::string(hovered_window->Name);
    } else {
        window_name = "None";
    }

    if (hovered_window) {
        if (strcmp(window_name.c_str(), "Sidebar") == 0) {
            hovered_element = HoveredUIElement::SIDEBAR;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::SIDEBAR];
        } else if (strcmp(window_name.c_str(), "Bottom Panel") == 0) {
            hovered_element = HoveredUIElement::BOTTOM_PANEL;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::BOTTOM_PANEL];
        } else if (strcmp(window_name.c_str(), "Ribbon") == 0) {
            hovered_element = HoveredUIElement::RIBBON;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::RIBBON];
        } else if (strcmp(window_name.c_str(), "Main Window") == 0) {
            hovered_element = HoveredUIElement::VIEWPORT;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::VIEWPORT];
        } else {
            hovered_element = HoveredUIElement::NONE;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
        }
        
    } else {
        hovered_element = HoveredUIElement::NONE;
        hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
    }
}

EngineEvent InputSystem::ingest_event(SDL_Event& e) {
    EngineEvent event;
    event.type = EngineEventType::None;

    static std::string event_type_dbg;
    static std::string capture_state;
    static int SDL_Scancode_dbg;
    DEBUG_HOOK_FUNCTION_NO_TIMER();
    DEBUG_HOOK_VAR_AS(event_type_dbg, "SDL_EVENT_TYPE");
    DEBUG_HOOK_VAR_AS(capture_state, "SDL_EVENT_CAPTURED");
    DEBUG_HOOK_VAR_AS(SDL_Scancode_dbg, "SDL_SCANCODE_NAME");

    ImGui_ImplSDL2_ProcessEvent(&e);
    if (e.type == SDL_QUIT || (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)){
        Debug::log("SDL_QUIT EVENT SEEN", DebugLevel::TRACE);
        set_closed();  // Set mode to closed
        event.type = EngineEventType::Quit;
        return event;
    }

    // Use existing hover detection - only pass events through if over viewport/canvas
    if (hovered_element != HoveredUIElement::VIEWPORT) {
        // Not over canvas/viewport - ImGui UI should handle this event
        event.type = EngineEventType::Captured;
        capture_state = "CAPTURED";
    } else {
        capture_state = "NOT_CAPTURED";
    }

    switch(e.type) {
    case(SDL_WINDOWEVENT) :
        // Handle window events - most should be filtered out as None
        switch(e.window.event) {
            case SDL_WINDOWEVENT_FOCUS_LOST:
                event_type_dbg = "FOCUS_LOST";
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                event_type_dbg = "FOCUS_GAINED";
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                event_type_dbg = "HIDDEN";
                event.type = EngineEventType::NoScreen;
                break;
            case SDL_WINDOWEVENT_SHOWN:
                event_type_dbg = "SHOWN";
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                event_type_dbg = "MINIMIZED";
                event.type = EngineEventType::NoScreen;
                break;
            case SDL_WINDOWEVENT_RESTORED:
                event_type_dbg = "RESTORED";
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                event_type_dbg = "EXPOSED";
                break;
            default:
                break;
        }
        // All window events return as None (filtered out)
        event.type = EngineEventType::None;
        break;
    case(SDL_MOUSEBUTTONDOWN) :
        event_type_dbg = "MOUSEBUTTONDOWN";
        event.type = EngineEventType::MouseButtonDown;
        event.mouse_button = e.button.button;
        event.mouse_x = e.button.x;
        event.mouse_y = e.button.y;
        break;
    case(SDL_MOUSEBUTTONUP) :
        event_type_dbg = "MOUSEBUTTONUP";
        event.type = EngineEventType::MouseButtonUp;
        event.mouse_button = e.button.button;
        event.mouse_x = e.button.x;
        event.mouse_y = e.button.y;
        break;
    case(SDL_MOUSEMOTION) :
        event_type_dbg = "MOUSEMOVE";
        event.type = EngineEventType::MouseMove;
        event.mouse_x = e.motion.x;
        event.mouse_y = e.motion.y;
        break;
    case(SDL_MOUSEWHEEL) :
        event_type_dbg = "MOUSEWHEEL";
        event.type = EngineEventType::MouseWheel;
        event.wheel_dy = e.wheel.y;
        event.wheel_direction_up = (e.wheel.y > 0);
        break;
    case(SDL_KEYDOWN) :
        event_type_dbg = "KEYDOWN";
        event.type = EngineEventType::KeyDown;
        event.key_info = e.key.keysym;
        SDL_Scancode_dbg = event.key_info.scancode;
        break;
    case(SDL_KEYUP) :
        event_type_dbg = "KEYUP";
        event.type = EngineEventType::KeyUp;
        event.key_info = e.key.keysym;
        SDL_Scancode_dbg = event.key_info.scancode;
        break;
    }

    switch(event.type) {
        case EngineEventType::KeyDown:
            if (event.key_info.scancode < SDL_NUM_SCANCODES && event.key_info.scancode >= 0) {
                key_states[event.key_info.scancode].update(true, latest_time);
                if (event.key_info.scancode == SDL_SCANCODE_LCTRL || event.key_info.scancode == SDL_SCANCODE_RCTRL) {
                    ctrl_state.update(true, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LSHIFT || event.key_info.scancode == SDL_SCANCODE_RSHIFT) {
                    shift_state.update(true, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LALT || event.key_info.scancode == SDL_SCANCODE_RALT) {
                    alt_state.update(true, latest_time);
                }
            }
            break;

        case EngineEventType::KeyUp:
            if (event.key_info.scancode < SDL_NUM_SCANCODES && event.key_info.scancode >= 0) {
                key_states[event.key_info.scancode].update(false, latest_time);
                if (event.key_info.scancode == SDL_SCANCODE_LCTRL || event.key_info.scancode == SDL_SCANCODE_RCTRL) {
                    ctrl_state.update(false, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LSHIFT || event.key_info.scancode == SDL_SCANCODE_RSHIFT) {
                    shift_state.update(false, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LALT || event.key_info.scancode == SDL_SCANCODE_RALT) {
                    alt_state.update(false, latest_time);
                }
            }
            break;

        case EngineEventType::MouseButtonDown:
            if (event.mouse_button == SDL_BUTTON_LEFT) {
                mouse_left_state.update(true, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_RIGHT) {
                mouse_right_state.update(true, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_MIDDLE) {
                mouse_middle_state.update(true, latest_time);
            }
            break;

        case EngineEventType::MouseButtonUp:
            if (event.mouse_button == SDL_BUTTON_LEFT) {
                mouse_left_state.update(false, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_RIGHT) {
                mouse_right_state.update(false, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_MIDDLE) {
                mouse_middle_state.update(false, latest_time);
            }
            break;

        case EngineEventType::MouseMove:
            update_mouse(static_cast<float>(event.mouse_x), static_cast<float>(event.mouse_y), latest_time);
            break;

        case EngineEventType::MouseWheel:
            mouse_wheel_dy.update(static_cast<float>(event.wheel_dy), latest_time);
            break;

        default:
            break;
    }


    return event;
}

void InputSystem::update_mouse(float new_mouse_x, float new_mouse_y, float timestamp) {
    float win_x = 0.0f;
    float win_y = 0.0f;
    
    switch(hovered_element) {
        case HoveredUIElement::VIEWPORT:
            win_x = viewport_x.value;
            win_y = viewport_y.value; 
            break;
        case HoveredUIElement::SIDEBAR:
            win_x = sidebar_x.value;
            win_y = sidebar_y.value;
            break;
        case HoveredUIElement::BOTTOM_PANEL:
            win_x = bottom_x.value;
            win_y = bottom_y.value;
            break;
        case HoveredUIElement::RIBBON:
            win_x = ribbon_x.value;
            win_y = ribbon_y.value;
            break;
        default:
            win_x = 0.0f;
            win_y = 0.0f;
            break;
    }
    
    mouse_x_relative.update(new_mouse_x - win_x, timestamp);
    mouse_y_relative.update(new_mouse_y - win_y, timestamp);
    mouse_x.update(new_mouse_x, timestamp);
    mouse_y.update(new_mouse_y, timestamp);
}

void InputSystem::save_accumulated_changes() {
    // Reset all change counters after SDL_PollEvents is complete
    ctrl_state.save_accumulated_changes();
    shift_state.save_accumulated_changes();
    alt_state.save_accumulated_changes();
    
    mouse_left_state.save_accumulated_changes();
    mouse_right_state.save_accumulated_changes();
    mouse_middle_state.save_accumulated_changes();
    
    mouse_wheel_dy.save_accumulated_changes();
    mouse_x.save_accumulated_changes();
    mouse_y.save_accumulated_changes();
    mouse_x_relative.save_accumulated_changes();
    mouse_y_relative.save_accumulated_changes();
    
    // Reset all individual key states
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        key_states[i].save_accumulated_changes();
    }
    
    // Reset UI element position tracking
    sidebar_x.save_accumulated_changes();
    sidebar_y.save_accumulated_changes();
    sidebar_width.save_accumulated_changes();
    sidebar_height.save_accumulated_changes();
    
    bottom_x.save_accumulated_changes();
    bottom_y.save_accumulated_changes();
    bottom_width.save_accumulated_changes();
    bottom_height.save_accumulated_changes();
    
    ribbon_x.save_accumulated_changes();
    ribbon_y.save_accumulated_changes();
    ribbon_width.save_accumulated_changes();
    ribbon_height.save_accumulated_changes();
    
    viewport_x.save_accumulated_changes();
    viewport_y.save_accumulated_changes();
    viewport_width.save_accumulated_changes();
    viewport_height.save_accumulated_changes();

    space_just_true = key_states[SDL_SCANCODE_SPACE].just_became_true();
    // Update window hover state once per frame
    // at the end of the frame so the dT loop has the latest mouse window location.
    update_window_hover();
}

void InputSystem::begin_frame(float timestamp) {
    // Update timestamp once at the start of the polling loop
    latest_time = timestamp;
}

void InputSystem::update_window_params(float& win_x, float& win_y, float& win_width, float& win_height, std::string window_name) {
    if (window_name == "Sidebar") {
        sidebar_x.update(win_x, latest_time);
        sidebar_y.update(win_y, latest_time);
        sidebar_width.update(win_width, latest_time);
        sidebar_height.update(win_height, latest_time);
    } else if (window_name == "Bottom Panel") {
        bottom_x.update(win_x, latest_time);
        bottom_y.update(win_y, latest_time);
        bottom_width.update(win_width, latest_time);
        bottom_height.update(win_height, latest_time);
    } else if (window_name == "Ribbon") {
        ribbon_x.update(win_x, latest_time);
        ribbon_y.update(win_y, latest_time);
        ribbon_width.update(win_width, latest_time);
        ribbon_height.update(win_height, latest_time);
    } else if (window_name == "Main Window") {
        viewport_x.update(win_x, latest_time);
        viewport_y.update(win_y, latest_time);
        viewport_width.update(win_width, latest_time);
        viewport_height.update(win_height, latest_time);
    } else {
        // Default case - no specific window handling needed
    }
}

void InputSystem::update_mode() {
    // Example mode update logic based on key states
    if (mouse_left_state.value) {
        if (!mouse_left_state.changed_this_frame()) {
            set_mode(UserMode::MODE_DRAG);
        }
    } else {
        clear_mode(UserMode::MODE_DRAG);
    }
}