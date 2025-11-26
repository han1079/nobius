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
    return true;
}

bool ModeUpdater::update_state_via_dT(float dT) {
    return true;
}
