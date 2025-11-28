#include <updaters/world_updater.h>
#include <updaters/orchestrator.h>

bool WorldUpdater::init() {
    return true;
}

bool WorldUpdater::shutdown() {
    return true;
}

bool WorldUpdater::update_state_via_event(EngineEvent &event) {
    // Get orchestrator instance
    

    return true;
}

bool WorldUpdater::update_state_via_dT(float dT) {
    return true;
}


uint64_t generate_uuid() {
    static std::random_device rd;
    static std::mt19937_64 rng(rd());
    static std::uniform_int_distribution<uint64_t> distr;

    return distr(rng);
}

