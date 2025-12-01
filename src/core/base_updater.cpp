#include <core/base_updater.h>

// Constructor
BaseUpdater::BaseUpdater() = default;

// Virtual destructor - must be defined for virtual base class
BaseUpdater::~BaseUpdater() = default;

// Virtual function implementations
bool BaseUpdater::init() {
    // Default implementation - can be overridden by derived classes
    return false;
}

bool BaseUpdater::shutdown() {
    // Default implementation - can be overridden by derived classes
    return false;
}

bool BaseUpdater::update_state_via_event() {
    // Default implementation - can be overridden by derived classes
    return false;
}

bool BaseUpdater::update_state_via_dT(float dT) {
    // Default implementation - can be overridden by derived classes
    return false;
}

bool BaseUpdater::submit_render_commands() {
    // Default implementation - can be overridden by derived classes
    return false;
}