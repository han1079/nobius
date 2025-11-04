#include <updaters/orchestrator.h>
#include <core/common.h>
#include <iostream>

/*Load Spec Constructor. Member objects find and build with load spec filename string*/
Orchestrator::Orchestrator(const LoadSpec& load_spec):
        m_mode_state(load_spec.mode_state_spec),
        m_world_state(load_spec.world_state_spec),
        m_renderer_state(load_spec.renderer_state_spec),
        m_mode_updater(m_mode_state),
        m_world_updater(m_world_state),
        m_renderer(m_renderer_state) {
        m_orchestrator_ptr = this;
    }

// Destructor implementation
Orchestrator::~Orchestrator() = default;

void Orchestrator::init(){
    // Recursive Init
    m_mode_updater.init();
    m_world_updater.init();
    m_renderer.init();
}

Orchestrator* Orchestrator::get(){
    return m_orchestrator_ptr;
}

void Orchestrator::run() {
    /*Main Loop Lives Here*/
    using clock = std::chrono::steady_clock;
    auto last_time = clock::now();
    auto last_frame_start = clock::now();
    SDL_Event event;
    int frame_count = 0;
    int event_ct = 0;
    while (m_running) {
        frame_count++;
        last_frame_start = clock::now();
        
        auto current_time = clock::now();
        std::chrono::duration<float> delta = current_time - last_time;
        last_time = current_time;
        float dT = delta.count();
        // Process ALL pending events, not just one
        while (SDL_PollEvent(&event)) {
            
            event_ct++;
            if (event.type != 0) {  // Only log non-empty events
                
            }
            
            /* Take latest input event, and update state:
             *
             * ModeUpdater:   ImGui steals input - if so, run its update and
             *                and read widget status to update local state anyway.
             *
             *                If ImGui doesn't steal input, parse the input and
             *                update local state via logic.
             *
             * WorldUpdater:  Direct entity update, placement, etc based on
             *                local state and captured event input
             *
             * Renderer:      Bulk renderer configuration update (shader modes, themes)
             * */
            m_mode_updater.update_state_via_event(event);
            m_world_updater.update_state_via_event(event);
            m_renderer.update_state_via_event(event);
        }

        m_renderer_state.set(&RendererState::event_count, event_ct);

        

        /* Take the dT and update time-dependent state:
         *
         * ModeUpdater:   Timeouts for bulk modes
         *
         * WorldUpdate:   All entity motion interpolation
         *
         * Renderer:      Runs the renderer pipeline submit
         * 
         * */
        
        
        m_mode_updater.update_state_via_dT(dT);
        
        m_world_updater.update_state_via_dT(dT);
        
        m_renderer.update_state_via_dT(dT);
        
        
        auto frame_end = clock::now();
        auto frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - last_frame_start);
        if (frame_duration.count() > 100) { // Warn if frame takes more than 100ms
            
        }
    }
}

void Orchestrator::shutdown() {
    m_running = false;
    /*Shutoff sequence*/
    m_mode_updater.shutdown();
    m_world_updater.shutdown();
    m_renderer.shutdown();
}




