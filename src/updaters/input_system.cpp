#include <updaters/input_system.h>
#include <imgui_internal.h>

InputSystem::InputSystem() 
{
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        keyboard.keys[i] = TimeStampedValue<bool>();
    }
    DEBUG_HOOK_FUNCTION_NO_TIMER();
    DEBUG_HOOK_VAR_AS(mouse.hovered_element_name, "HOVERED_UI_ELEMENT_NAME");
    DEBUG_HOOK_VAR_AS(mouse.screen_pos.value.x, "MOUSE_X");
    DEBUG_HOOK_VAR_AS(mouse.screen_pos.value.y, "MOUSE_Y");
    DEBUG_HOOK_VAR_AS(mouse.relative_pos.value.x, "MOUSE_X_RELATIVE");
    DEBUG_HOOK_VAR_AS(mouse.relative_pos.value.y, "MOUSE_Y_RELATIVE");
    DEBUG_HOOK_VAR_AS(user_mode, "USER_MODE_FLAGS");
    DEBUG_HOOK_VAR_AS(keyboard.keys[SDL_SCANCODE_SPACE].value, "KEY_SPACE_STATE");
    DEBUG_HOOK_VAR_AS(space_just_true, "KEY_SPACE_JUST_TRUE");
}

void InputSystem::update_time(float timestamp) {
    latest_time = timestamp;
}

bool InputSystem::is_key_pressed(SDL_Scancode key) const {
    if (key >= 0 && key < SDL_NUM_SCANCODES) {
        return keyboard.keys[key].value;
    }
    return false;
}

bool InputSystem::just_pressed(SDL_Scancode key) const {
    if (key >= 0 && key < SDL_NUM_SCANCODES) {
        return keyboard.keys[key].just_became_true();
    }
    return false;
}

bool InputSystem::is_mouse_pressed(int button) const {
    switch(button) {
        case SDL_BUTTON_LEFT: return mouse.left_button.value;
        case SDL_BUTTON_RIGHT: return mouse.right_button.value;
        case SDL_BUTTON_MIDDLE: return mouse.middle_button.value;
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
            mouse.hovered_element = HoveredUIElement::SIDEBAR;
            mouse.hovered_element_name = HoveredUIElementNames[HoveredUIElement::SIDEBAR];
        } else if (strcmp(window_name.c_str(), "Bottom Panel") == 0) {
            hovered_element = HoveredUIElement::BOTTOM_PANEL;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::BOTTOM_PANEL];
            mouse.hovered_element = HoveredUIElement::BOTTOM_PANEL;
            mouse.hovered_element_name = HoveredUIElementNames[HoveredUIElement::BOTTOM_PANEL];
        } else if (strcmp(window_name.c_str(), "Ribbon") == 0) {
            hovered_element = HoveredUIElement::RIBBON;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::RIBBON];
            mouse.hovered_element = HoveredUIElement::RIBBON;
            mouse.hovered_element_name = HoveredUIElementNames[HoveredUIElement::RIBBON];
        } else if (strcmp(window_name.c_str(), "Main Window") == 0) {
            hovered_element = HoveredUIElement::VIEWPORT;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::VIEWPORT];
            mouse.hovered_element = HoveredUIElement::VIEWPORT;
            mouse.hovered_element_name = HoveredUIElementNames[HoveredUIElement::VIEWPORT];
        } else {
            hovered_element = HoveredUIElement::NONE;
            hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
            mouse.hovered_element = HoveredUIElement::NONE;
            mouse.hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
        }
        
    } else {
        hovered_element = HoveredUIElement::NONE;
        hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
        mouse.hovered_element = HoveredUIElement::NONE;
        mouse.hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
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
                keyboard.keys[event.key_info.scancode].update(true, latest_time);
                if (event.key_info.scancode == SDL_SCANCODE_LCTRL || event.key_info.scancode == SDL_SCANCODE_RCTRL) {
                    keyboard.ctrl.update(true, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LSHIFT || event.key_info.scancode == SDL_SCANCODE_RSHIFT) {
                    keyboard.shift.update(true, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LALT || event.key_info.scancode == SDL_SCANCODE_RALT) {
                    keyboard.alt.update(true, latest_time);
                }
            }
            break;

        case EngineEventType::KeyUp:
            if (event.key_info.scancode < SDL_NUM_SCANCODES && event.key_info.scancode >= 0) {
                keyboard.keys[event.key_info.scancode].update(false, latest_time);
                if (event.key_info.scancode == SDL_SCANCODE_LCTRL || event.key_info.scancode == SDL_SCANCODE_RCTRL) {
                    keyboard.ctrl.update(false, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LSHIFT || event.key_info.scancode == SDL_SCANCODE_RSHIFT) {
                    keyboard.shift.update(false, latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LALT || event.key_info.scancode == SDL_SCANCODE_RALT) {
                    keyboard.alt.update(false, latest_time);
                }
            }
            break;

        case EngineEventType::MouseButtonDown:
            if (event.mouse_button == SDL_BUTTON_LEFT) {
                mouse.left_button.update(true, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_RIGHT) {
                mouse.right_button.update(true, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_MIDDLE) {
                mouse.middle_button.update(true, latest_time);
            }
            break;

        case EngineEventType::MouseButtonUp:
            if (event.mouse_button == SDL_BUTTON_LEFT) {
                mouse.left_button.update(false, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_RIGHT) {
                mouse.right_button.update(false, latest_time);
            } else if (event.mouse_button == SDL_BUTTON_MIDDLE) {
                mouse.middle_button.update(false, latest_time);
            }
            break;

        case EngineEventType::MouseMove:
            update_mouse(static_cast<float>(event.mouse_x), static_cast<float>(event.mouse_y), latest_time);
            break;

        case EngineEventType::MouseWheel:
            mouse.wheel_delta.update(static_cast<float>(event.wheel_dy), latest_time);
            break;

        default:
            break;
    }


    return event;
}

void InputSystem::update_mouse(float new_mouse_x, float new_mouse_y, float timestamp) {
    float win_x = 0.0f;
    float win_y = 0.0f;
    float cartesian_y = 0.0f;
    float cartesian_x = 0.0f;
    
    win_x = windows.viewport.value.x;
    win_y = windows.viewport.value.y; 
    cartesian_x = new_mouse_x - win_x;
    cartesian_y = windows.viewport.value.w - (new_mouse_y - win_y);

    // Update structured state
    mouse.screen_pos.update(glm::vec2(new_mouse_x, new_mouse_y), timestamp);
    mouse.relative_pos.update(glm::vec2(new_mouse_x - win_x, new_mouse_y - win_y), timestamp);
    mouse.cartesian_pos.update(glm::vec2(cartesian_x, cartesian_y), timestamp);
    // Delta is calculated automatically by TimeStampedValue
}

void InputSystem::save_accumulated_changes() {
    // Reset all change counters after SDL_PollEvents is complete
    keyboard.ctrl.save_accumulated_changes();
    keyboard.shift.save_accumulated_changes();
    keyboard.alt.save_accumulated_changes();
    
    mouse.left_button.save_accumulated_changes();
    mouse.right_button.save_accumulated_changes();
    mouse.middle_button.save_accumulated_changes();
    
    mouse.wheel_delta.save_accumulated_changes();
    mouse.screen_pos.save_accumulated_changes();
    mouse.relative_pos.save_accumulated_changes();
    mouse.cartesian_pos.save_accumulated_changes();
    
    // Reset all individual key states
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        keyboard.keys[i].save_accumulated_changes();
    }
    
    // Reset UI element position tracking
    windows.sidebar.save_accumulated_changes();
    windows.bottom_panel.save_accumulated_changes();
    windows.ribbon.save_accumulated_changes();
    windows.viewport.save_accumulated_changes();

    space_just_true = keyboard.keys[SDL_SCANCODE_SPACE].just_became_true();
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
        windows.sidebar.update(glm::vec4(win_x, win_y, win_width, win_height), latest_time);
    } else if (window_name == "Bottom Panel") {
        windows.bottom_panel.update(glm::vec4(win_x, win_y, win_width, win_height), latest_time);
    } else if (window_name == "Ribbon") {
        windows.ribbon.update(glm::vec4(win_x, win_y, win_width, win_height), latest_time);
    } else if (window_name == "Main Window") {
        windows.viewport.update(glm::vec4(win_x, win_y, win_width, win_height), latest_time);
    } else {
        // Default case - no specific window handling needed
    }
}

void InputSystem::update_mode() {
    // Example mode update logic based on key states
    if (mouse.left_button.value) {
        if (!mouse.left_button.changed_this_frame()) {
            set_mode(UserMode::MODE_DRAG);
        }
    } else {
        clear_mode(UserMode::MODE_DRAG);
    }
}