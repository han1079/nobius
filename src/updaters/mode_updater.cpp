#include <updaters/mode_updater.h>
#include <updaters/orchestrator.h>

ModeUpdater::ModeUpdater(ModeState& state) : m_mode_data(state) {}

bool ModeUpdater::init() {
    return true;
}

bool ModeUpdater::shutdown() {
    return true;
}

bool ModeUpdater::update_state_via_event(EngineEvent &event) {
    Orchestrator* orchestrator = Orchestrator::get(); 
    // Get references to orchestrator states
    const auto& mode_state = orchestrator->get_mode_state();
    const auto& imgui_state = orchestrator->get_imgui_state();
    
    if (event.type == EngineEventType::Quit) {
        m_mode_data.set_closed();
        return true;
    }
    // Handle mouse click events
  
    return true;
}

bool ModeUpdater::update_state_via_dT(float dT) {auto* orchestrator = Orchestrator::get();
 
    return true;
}
