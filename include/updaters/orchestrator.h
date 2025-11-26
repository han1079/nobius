
#pragma once
#include <pch.h>
#include <core/common.h>

#include <state/imgui_state.h>
#include <state/mode_state.h>
#include <state/world_state.h>

#include <updaters/mode_updater.h>
#include <updaters/world_updater.h>
#include <updaters/imgui_updater.h>
#include <updaters/event_ingester.h>

#include <core/renderer.h>



class Orchestrator {

public:

    //Constructor - can be default or with LoadSpec
    Orchestrator() = delete;
    Orchestrator(const LoadSpec& load_spec);

    //TODO: make actual destructor. For now, I'm leaving it empty because
    // quitting Orchestrator means main ends.
    ~Orchestrator();

    //Getter Function for the Orchestrator
    static Orchestrator* get();

    void init();
    void run();
    void sig_exit_loop();
    void shutdown();
    
    // Getter methods
    const ImGuiState& get_imgui_state() const { return m_imgui_state; }
    const ModeState& get_mode_state() const { return m_mode_state; }
    const WorldState& get_world_state() const { return m_world_state; }
    const UIState& get_ui_state() const { return m_ui_state; }
    
private:

    bool m_running = true;

    // Inline static pointer -> one global instance. Shared across everything that
    // includes Orchestrator.h or needs to access this object. 
    inline static Orchestrator* m_orchestrator_ptr = nullptr;

    /*The Orchestrator owns the following state trackers
     *
     * UIState - UI Interaction States (hovered elements, mouse pos, key states)
     * ModeState - Bulk Modes (Edit Modes, window minimized, pause/run, etc)
     * WorldState - Entities, Scene Data, dT steps
     * ImGuiState - Window configuration, shader code strings, etc
     *
     * */

    UIState m_ui_state;
    ModeState m_mode_state;
    WorldState m_world_state;
    ImGuiState m_imgui_state;

    /*The Orchestrator owns the following control objects:
     * 
     * ModeUpdater
     * WorldUpdater
     * ImGuiUpdater
     * EventIngester
     * 
     * */
    EventIngester m_event_ingester;
    ModeUpdater m_mode_updater;
    WorldUpdater m_world_updater;
    ImGuiUpdater m_imgui_updater;

    Renderer m_renderer;
    friend class Renderer;
};

