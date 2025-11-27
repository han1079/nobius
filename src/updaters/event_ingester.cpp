#include <updaters/event_ingester.h>
#include <imgui_internal.h>

void EventIngester::update_time(float timestamp) {
    m_ui_state.latest_time = timestamp;
}

void EventIngester::update_window_hover() {
    ImGuiWindow* hovered_window = ImGui::GetCurrentContext()->HoveredWindow;
    std::string window_name;
    
    if (hovered_window) {
        window_name = std::string(hovered_window->Name);
    } else {
        window_name = "None";
    }

    
    
    if (hovered_window) {
        std::cout << "Hovered window: " << window_name.c_str() << std::endl; 
        if (strcmp(window_name.c_str(), "Sidebar") == 0) {
            m_ui_state.hovered_element = HoveredUIElement::SIDEBAR;
            m_ui_state.hovered_element_name = HoveredUIElementNames[HoveredUIElement::SIDEBAR];
        } else if (strcmp(window_name.c_str(), "Bottom Panel") == 0) {
            m_ui_state.hovered_element = HoveredUIElement::BOTTOM_PANEL;
            m_ui_state.hovered_element_name = HoveredUIElementNames[HoveredUIElement::BOTTOM_PANEL];
        } else if (strcmp(window_name.c_str(), "Ribbon") == 0) {
            m_ui_state.hovered_element = HoveredUIElement::RIBBON;
            m_ui_state.hovered_element_name = HoveredUIElementNames[HoveredUIElement::RIBBON];
        } else if (strcmp(window_name.c_str(), "Main Window") == 0) {
            m_ui_state.hovered_element = HoveredUIElement::VIEWPORT;
            m_ui_state.hovered_element_name = HoveredUIElementNames[HoveredUIElement::VIEWPORT];
        } else {
            m_ui_state.hovered_element = HoveredUIElement::NONE;
            m_ui_state.hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
        }
        
    } else {
        std::cout << "Hovered window: None" << std::endl;
        m_ui_state.hovered_element = HoveredUIElement::NONE;
        m_ui_state.hovered_element_name = HoveredUIElementNames[HoveredUIElement::NONE];
    }
}

EngineEvent EventIngester::ingest_event(SDL_Event& e) {
    EngineEvent event;
    event.type = EngineEventType::None;

    static std::string event_type_dbg;
    static std::string capture_state;
    DEBUG_HOOK_FUNCTION_NO_TIMER();
    DEBUG_HOOK_VAR_AS(event_type_dbg, "SDL_EVENT_TYPE");
    DEBUG_HOOK_VAR_AS(capture_state, "SDL_EVENT_CAPTURED");


    ImGui_ImplSDL2_ProcessEvent(&e);
    if (e.type == SDL_QUIT || (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)){
        Debug::log("SDL_QUIT EVENT SEEN", DebugLevel::TRACE);
        event.type = EngineEventType::Quit;
        return event;
    }

    // Use existing hover detection - only pass events through if over viewport/canvas
    if (m_ui_state.hovered_element != HoveredUIElement::VIEWPORT) {
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
        event_type_dbg = "MOUSEKEYDOWN";
        event.type = EngineEventType::KeyDown;
        event.key_info = e.key.keysym;
        break;
    case(SDL_KEYUP) :
        event_type_dbg = "MOUSEKEYUP";
        event.type = EngineEventType::KeyUp;
        event.key_info = e.key.keysym;
        break;
    }

    switch(event.type) {
        case EngineEventType::KeyDown:
            if (event.key_info.scancode < SDL_NUM_SCANCODES && event.key_info.scancode >= 0) {
                m_ui_state.KeyStates[event.key_info.scancode].update(true, m_ui_state.latest_time);
                if (event.key_info.scancode == SDL_SCANCODE_LCTRL || event.key_info.scancode == SDL_SCANCODE_RCTRL) {
                    m_ui_state.ctrl_state.update(true, m_ui_state.latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LSHIFT || event.key_info.scancode == SDL_SCANCODE_RSHIFT) {
                    m_ui_state.shift_state.update(true, m_ui_state.latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LALT || event.key_info.scancode == SDL_SCANCODE_RALT) {
                    m_ui_state.alt_state.update(true, m_ui_state.latest_time);
                }
            }
            break;

        case EngineEventType::KeyUp:
            if (event.key_info.scancode < SDL_NUM_SCANCODES && event.key_info.scancode >= 0) {
                m_ui_state.KeyStates[event.key_info.scancode].update(false, m_ui_state.latest_time);
                if (event.key_info.scancode == SDL_SCANCODE_LCTRL || event.key_info.scancode == SDL_SCANCODE_RCTRL) {
                    m_ui_state.ctrl_state.update(false, m_ui_state.latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LSHIFT || event.key_info.scancode == SDL_SCANCODE_RSHIFT) {
                    m_ui_state.shift_state.update(false, m_ui_state.latest_time);
                } else if (event.key_info.scancode == SDL_SCANCODE_LALT || event.key_info.scancode == SDL_SCANCODE_RALT) {
                    m_ui_state.alt_state.update(false, m_ui_state.latest_time);
                }
            }
            break;

        case EngineEventType::MouseButtonDown:
            if (event.mouse_button == SDL_BUTTON_LEFT) {
                m_ui_state.mouse_left_state.update(true, m_ui_state.latest_time);
            } else if (event.mouse_button == SDL_BUTTON_RIGHT) {
                m_ui_state.mouse_right_state.update(true, m_ui_state.latest_time);
            } else if (event.mouse_button == SDL_BUTTON_MIDDLE) {
                m_ui_state.mouse_middle_state.update(true, m_ui_state.latest_time);
            }
            break;

        case EngineEventType::MouseButtonUp:
            if (event.mouse_button == SDL_BUTTON_LEFT) {
                m_ui_state.mouse_left_state.update(false, m_ui_state.latest_time);
            } else if (event.mouse_button == SDL_BUTTON_RIGHT) {
                m_ui_state.mouse_right_state.update(false, m_ui_state.latest_time);
            } else if (event.mouse_button == SDL_BUTTON_MIDDLE) {
                m_ui_state.mouse_middle_state.update(false, m_ui_state.latest_time);
            }
            break;

        case EngineEventType::MouseMove:
            update_mouse(static_cast<float>(event.mouse_x), static_cast<float>(event.mouse_y), m_ui_state.latest_time);
            break;

        case EngineEventType::MouseWheel:
            m_ui_state.mouse_wheel_dy.update(static_cast<float>(event.wheel_dy), m_ui_state.latest_time);
            break;

        default:
            break;
    }


    return event;
}

void EventIngester::update_mouse(float new_mouse_x, float new_mouse_y, float timestamp) {
    float win_x = 0.0f;
    float win_y = 0.0f;
    
    switch(m_ui_state.hovered_element) {
        case HoveredUIElement::VIEWPORT:
            win_x = m_ui_state.viewport_x.value;
            win_y = m_ui_state.viewport_y.value; 
            break;
        case HoveredUIElement::SIDEBAR:
            win_x = m_ui_state.sidebar_x.value;
            win_y = m_ui_state.sidebar_y.value;
            break;
        case HoveredUIElement::BOTTOM_PANEL:
            win_x = m_ui_state.bottom_x.value;
            win_y = m_ui_state.bottom_y.value;
            break;
        case HoveredUIElement::RIBBON:
            win_x = m_ui_state.ribbon_x.value;
            win_y = m_ui_state.ribbon_y.value;
            break;
        default:
            win_x = 0.0f;
            win_y = 0.0f;
            break;
    }
    
    m_ui_state.mouse_x_relative.update(new_mouse_x - win_x, timestamp);
    m_ui_state.mouse_y_relative.update(new_mouse_y - win_y, timestamp);
    m_ui_state.mouse_x.update(new_mouse_x, timestamp);
    m_ui_state.mouse_y.update(new_mouse_y, timestamp);
}

void EventIngester::reset_changes() {
    // Reset all change counters after SDL_PollEvents is complete
    m_ui_state.ctrl_state.reset_changes();
    m_ui_state.shift_state.reset_changes();
    m_ui_state.alt_state.reset_changes();
    
    m_ui_state.mouse_left_state.reset_changes();
    m_ui_state.mouse_right_state.reset_changes();
    m_ui_state.mouse_middle_state.reset_changes();
    
    m_ui_state.mouse_wheel_dy.reset_changes();
    m_ui_state.mouse_x.reset_changes();
    m_ui_state.mouse_y.reset_changes();
    m_ui_state.mouse_x_relative.reset_changes();
    m_ui_state.mouse_y_relative.reset_changes();
    
    // Reset all individual key states
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        m_ui_state.KeyStates[i].reset_changes();
    }
    
    // Reset UI element position tracking
    m_ui_state.sidebar_x.reset_changes();
    m_ui_state.sidebar_y.reset_changes();
    m_ui_state.sidebar_width.reset_changes();
    m_ui_state.sidebar_height.reset_changes();
    
    m_ui_state.bottom_x.reset_changes();
    m_ui_state.bottom_y.reset_changes();
    m_ui_state.bottom_width.reset_changes();
    m_ui_state.bottom_height.reset_changes();
    
    m_ui_state.ribbon_x.reset_changes();
    m_ui_state.ribbon_y.reset_changes();
    m_ui_state.ribbon_width.reset_changes();
    m_ui_state.ribbon_height.reset_changes();
    
    m_ui_state.viewport_x.reset_changes();
    m_ui_state.viewport_y.reset_changes();
    m_ui_state.viewport_width.reset_changes();
    m_ui_state.viewport_height.reset_changes();

    // Update window hover state once per frame
    // at the end of the frame so the dT loop has the latest mouse window location.
    update_window_hover();
}

void EventIngester::begin_frame(float timestamp) {
    // Update timestamp once at the start of the polling loop
    m_ui_state.latest_time = timestamp;
    
}

void EventIngester::update_window_params(float& win_x, float& win_y, float& win_width, float& win_height, std::string window_name) {
    if (window_name == "Sidebar") {
        m_ui_state.sidebar_x.update(win_x, m_ui_state.latest_time);
        m_ui_state.sidebar_y.update(win_y, m_ui_state.latest_time);
        m_ui_state.sidebar_width.update(win_width, m_ui_state.latest_time);
        m_ui_state.sidebar_height.update(win_height, m_ui_state.latest_time);
    } else if (window_name == "Bottom Panel") {
        m_ui_state.bottom_x.update(win_x, m_ui_state.latest_time);
        m_ui_state.bottom_y.update(win_y, m_ui_state.latest_time);
        m_ui_state.bottom_width.update(win_width, m_ui_state.latest_time);
        m_ui_state.bottom_height.update(win_height, m_ui_state.latest_time);
    } else if (window_name == "Ribbon") {
        m_ui_state.ribbon_x.update(win_x, m_ui_state.latest_time);
        m_ui_state.ribbon_y.update(win_y, m_ui_state.latest_time);
        m_ui_state.ribbon_width.update(win_width, m_ui_state.latest_time);
        m_ui_state.ribbon_height.update(win_height, m_ui_state.latest_time);
    } else if (window_name == "Main Window") {
        m_ui_state.viewport_x.update(win_x, m_ui_state.latest_time);
        m_ui_state.viewport_y.update(win_y, m_ui_state.latest_time);
        m_ui_state.viewport_width.update(win_width, m_ui_state.latest_time);
        m_ui_state.viewport_height.update(win_height, m_ui_state.latest_time);
    } else {
        // Default case - no specific window handling needed
    }
}