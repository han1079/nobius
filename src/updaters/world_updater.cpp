#include <updaters/world_updater.h>
#include <updaters/orchestrator.h>

// Baseline Constructor - initialize with a default world state object
WorldUpdater::WorldUpdater(WorldState& state) : m_world_data(state) {}

bool WorldUpdater::init() {
    return true;
}

bool WorldUpdater::shutdown() {
    return true;
}

bool WorldUpdater::update_state_via_event(EngineEvent &event) {
    // Get orchestrator instance
    auto* orchestrator = Orchestrator::get();
    if (!orchestrator) return true;
    
    // Get references to orchestrator states
    const auto& mode_state = orchestrator->get_mode_state();
    const auto& imgui_state = orchestrator->get_imgui_state();
    
    // Check if we're in the right mode for entity placement
    // Since user_mode is private, we'll need to add a public getter or make WorldUpdater a friend
    // For now, let's assume we're in placement mode and mouse is in canvas
    bool mouse_in_canvas = true; // TODO: Implement proper canvas hit detection
    bool in_place_mode = true;   // TODO: Check actual mode state
    
    // Handle mouse click events
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (mouse_in_canvas && in_place_mode) {
            // Get mouse position
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);
            
            // For now, just print the mouse coordinates
            // TODO: Convert to canvas coordinates when canvas members are accessible
            printf("Mouse clicked at screen position: (%d, %d)\n", mouse_x, mouse_y);
            
            // TODO: Create new entity when Entity constructor is available
            // Entity new_entity(generate_uuid()); // Example constructor call
            
            printf("TODO: Create new entity at mouse position\n");
        }
    }

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

