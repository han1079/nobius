
#pragma once
#include <pch.h>
#include <core/common.h>

#include <state/system_state.h>
#include <state/window_state.h>
#include <state/world_state.h>

#include <updaters/state_updater.h>
#include <updaters/world_updater.h>
#include <updaters/renderer.h>



class Orchestrator {
private:
    void init();
    void run();
    static Orchestrator* orch_inst_ptr;

public:

    //Constructor and Destructor for the high level orchestrator
    Orchestrator(std::string config_file_name) : 
        m_config_file(config_file_name),
        m_state_updater(),
        m_world_updater(),
        m_renderer(),
        m_system_state(),
        m_world_state(),
        m_running(true) {
        m_orchestrator_ptr = this;
    }
    //TODO: make actual destructor. For now, I'm leaving it empty because
    // quitting Orchestrator means main ends.
    ~Orchestrator() = default;

    //Getter Function for the Orchestrator
    static Orchestrator* get();

private:
    
    

    bool m_running;
    std::string m_config_file;
    static Orchestrator* m_orchestrator_ptr;

    /*The Orchestrator owns the following control objects:
     * 
     * StateUpdater
     * EntityUpdater
     * Renderer
     * 
     * */
    StateUpdater m_state_updater;
    WorldUpdater m_world_updater;
    Renderer m_renderer;

    /*The Orchestrator owns the following state trackers
     *
     * SystemState
     * SceneData
     *
     * */

    SystemState m_system_state;
    WorldState m_world_state;
    WindowState m_window_state;

};

