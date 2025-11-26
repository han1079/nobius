#include <updaters/event_ingester.h>
#include <imgui_internal.h>

void EventIngester::update_time(float timestamp) {
    m_ui_state.latest_time = timestamp;
}

void EventIngester::update_window_hover() {
    ImGuiWindow* hovered_window = ImGui::GetCurrentContext()->HoveredWindow;
    if (hovered_window) {
        ImGuiID window_id = hovered_window->ID;
        static const ImGuiID sidebar_id = ImHashStr("Sidebar");
        static const ImGuiID bottom_id = ImHashStr("Bottom Panel");
        static const ImGuiID ribbon_id = ImHashStr("Ribbon");
        static const ImGuiID viewport_id = ImHashStr("Viewport");
        
        if (window_id == sidebar_id) {
            m_ui_state.hovered_element = SIDEBAR;
        } else if (window_id == bottom_id) {
            m_ui_state.hovered_element = BOTTOM_PANEL;
        } else if (window_id == ribbon_id) {
            m_ui_state.hovered_element = RIBBON;
        } else if (window_id == viewport_id) {
            m_ui_state.hovered_element = VIEWPORT;
        } else {
            m_ui_state.hovered_element = NONE;
        }
    }
}

EngineEvent EventIngester::ingest_event(SDL_Event& e) {
    EngineEvent event;
    event.type = EngineEventType::None;

    ImGui_ImplSDL2_ProcessEvent(&e);
    if (e.type == SDL_QUIT || (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)){
        event.type = EngineEventType::Quit;
        return event;
    }

    ImGuiIO& io = ImGui::GetIO();
    
    if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
        // ImGui wants to capture the event, so we flag it to not propagate.
        event.type = EngineEventType::Captured;
    }

    switch(e.type) {
    case(SDL_MOUSEBUTTONDOWN) :
        event.type = EngineEventType::MouseButtonDown;
        event.mouse_button = e.button.button;
        event.mouse_x = e.button.x;
        event.mouse_y = e.button.y;
        break;
    case(SDL_MOUSEBUTTONUP) :
        event.type = EngineEventType::MouseButtonUp;
        event.mouse_button = e.button.button;
        event.mouse_x = e.button.x;
        event.mouse_y = e.button.y;
        break;
    case(SDL_MOUSEMOTION) :
        event.type = EngineEventType::MouseMove;
        event.mouse_x = e.motion.x;
        event.mouse_y = e.motion.y;
        break;
    case(SDL_MOUSEWHEEL) :
        event.type = EngineEventType::MouseWheel;
        event.wheel_dy = e.wheel.y;
        event.wheel_direction_up = (e.wheel.y > 0);
        break;
    case(SDL_KEYDOWN) :
        event.type = EngineEventType::KeyDown;
        event.key_info = e.key.keysym;
        break;
    case(SDL_KEYUP) :
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
        case VIEWPORT:
            win_x = m_ui_state.viewport_x.value;
            win_y = m_ui_state.viewport_y.value; 
            break;
        case SIDEBAR:
            win_x = m_ui_state.sidebar_x.value;
            win_y = m_ui_state.sidebar_y.value;
            break;
        case BOTTOM_PANEL:
            win_x = m_ui_state.bottom_x.value;
            win_y = m_ui_state.bottom_y.value;
            break;
        case RIBBON:
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
