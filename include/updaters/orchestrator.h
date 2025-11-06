
#pragma once
#include <pch.h>
#include <core/common.h>

#include <state/imgui_state.h>
#include <state/mode_state.h>
#include <state/world_state.h>

#include <updaters/mode_updater.h>
#include <updaters/world_updater.h>
#include <updaters/imgui_updater.h>



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
private:

    bool m_running = true;

    // Inline static pointer -> one global instance. Shared across everything that
    // includes Orchestrator.h or needs to access this object. 
    inline static Orchestrator* m_orchestrator_ptr = nullptr;

    /*The Orchestrator owns the following state trackers
     *
     * ModeState - Bulk Modes (Edit Modes, window minimized, pause/run, etc)
     * WorldState - Entities, Scene Data, dT steps
     * ImGuiState - Window configuration, shader code strings, etc
     *
     * */

    ModeState m_mode_state;
    WorldState m_world_state;
    ImGuiState m_imgui_state;

    /*The Orchestrator owns the following control objects:
     * 
     * ModeUpdater
     * WorldUpdater
     * ImGuiUpdater
     * 
     * */
    ModeUpdater m_mode_updater;
    WorldUpdater m_world_updater;
    ImGuiUpdater m_imgui_updater;

};

