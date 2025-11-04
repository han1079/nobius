#include <updaters/world_updater.h>

// Baseline Constructor - initialize with a default world state object
WorldUpdater::WorldUpdater(WorldState& state) : m_world_data(state) {}

bool WorldUpdater::init() {
    return true;
}

bool WorldUpdater::shutdown() {
    return true;
}

bool WorldUpdater::update_state_via_event(SDL_Event &event) {
    return true;
}

bool WorldUpdater::update_state_via_dT(float dT) {
    return true;
}

