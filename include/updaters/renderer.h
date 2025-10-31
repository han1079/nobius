#pragma once

#include <pch.h>
#include <core/common.h>
#include <core/base_updater.h>
#include <state/window_state.h>


class Renderer : public BaseUpdater {
public:

    
    Renderer() = default;
    ~Renderer() = default;

    friend Orchestrator;
private:
    WindowState m_cfg;

    // Basic Window Configuration Data
    
    int win_width;
    int win_height;
    SDL_Window* window;
    SDL_GLContext gl_context;


protected:
    Renderer(WindowState& config) : m_cfg{config} {}

    bool init() override;
    bool shutdown() override;
};
