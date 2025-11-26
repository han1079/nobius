#pragma once
#include <pch.h>
#include <core/common.h>
#include <state/mode_state.h>

class EventIngester {
public:
    EventIngester(UIState& st) : m_ui_state(st) {}
    ~EventIngester() = default;

private:
    UIState& m_ui_state;
    void update_mouse(float new_mouse_x, float new_mouse_y, float timestamp);

public:
    void update_time(float timestamp);  // Call once at start of SDL_PollEvent loop
    void update_window_hover();
    EngineEvent ingest_event(SDL_Event& event);
    
    // Frame management
    void reset_changes();  // Call after SDL_PollEvents completes
    void begin_frame(float timestamp);  // Call once before event processing starts
};
