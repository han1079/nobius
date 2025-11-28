#include <updaters/orchestrator.h>
#include <core/common.h>
#include <iostream>

/*Load Spec Constructor. Member objects find and build with load spec filename string*/
Orchestrator::Orchestrator(const LoadSpec& load_spec):
        m_imgui_state(load_spec.imgui_state_spec),
        m_input_system(),
        m_world_updater(),
        m_imgui_updater(m_imgui_state) {
        m_orchestrator_ptr = this;
    }

// Destructor implementation
Orchestrator::~Orchestrator() = default;

void Orchestrator::init(){
    // Recursive Init
    m_world_updater.init();
    m_imgui_updater.init();
    
    // Initialize renderer with GLAD - this must happen after SDL context creation
    if (!m_renderer.init()) {
        Debug::log("Failed to initialize renderer (GLAD initialization failed)", DebugLevel::FATAL);
        m_running = false;
        return;
    }
}

Orchestrator* Orchestrator::get(){
    return m_orchestrator_ptr;
}

void Orchestrator::sig_exit_loop() {
    m_running = false;
}

void Orchestrator::run() {
    /*Main Loop Lives Here*/
    Instrumentor::Get().BeginSession("Main Loop", "results.json");
    using clock = std::chrono::steady_clock;
    auto last_time = clock::now();
    auto last_frame_start = clock::now();
    auto current_time = clock::now();

    SDL_Event sdl_event;
    EngineEvent event;
    int frame_count = 0;
    int total_event_ct = 0;
    int loop_event_ct = 0;
    while (m_running) {

        

        frame_count++;
        last_frame_start = clock::now();
        
        current_time = clock::now();
        std::chrono::duration<float> delta = current_time - last_time;
        last_time = current_time;
        float dT = delta.count();

        m_input_system.update_time(current_time.time_since_epoch().count());
        m_imgui_updater.draw_gui();
        // Process ALL pending events, not just one
        while (SDL_PollEvent(&sdl_event)) {
            TIME_LOCATION();
            total_event_ct++; 
            loop_event_ct++;

            event = m_input_system.ingest_event(sdl_event);
            if (event.type == EngineEventType::NoScreen) {
                // Screen is not visible. Skip any event processing
                continue;
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
             * ImGuiUpdater:  Bulk renderer configuration update (shader modes, themes)
             * */
            m_world_updater.update_state_via_event(event);
            m_imgui_updater.update_state_via_event(event);
        }

        m_input_system.reset_changes();
        loop_event_ct = 0;
        m_imgui_state.set(&ImGuiState::event_count, total_event_ct);
        

        /* Take the dT and update time-dependent state:
         *
         * ModeUpdater:   Timeouts for bulk modes
         *
         * WorldUpdate:   All entity motion interpolation
         *
         * ImGuiUpdater:  Runs the renderer pipeline submit
         * 
         * */
        
        m_world_updater.update_state_via_dT(dT);
        
        m_imgui_updater.update_state_via_dT(dT);
        /* Render Pass */

        m_renderer.start_frame();
        m_renderer.render_frame();
        m_renderer.end_frame();

        
        
        // Check if window is visible before swapping buffers
        Uint32 window_flags = SDL_GetWindowFlags(m_imgui_updater.window);
        bool should_swap = !(window_flags & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN));
        
        if (should_swap) {
            SDL_GL_SwapWindow(m_imgui_updater.window);        

        } else {
            // Window is occluded, sleep a bit to avoid busy loop
            SDL_Delay(16); // ~60 FPS equivalent
        }
        auto frame_end = clock::now();
        auto frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - last_frame_start);
        if (frame_duration.count() > 100) { // Warn if frame takes more than 100ms
            Debug::log("Warning: Frame took " + std::to_string(frame_duration.count()) + " ms", DebugLevel::WARN);
        }

        if (!m_input_system.get_mode_flags()) {
            Debug::log("Close requested via InputSystem flag. Exiting main loop.", DebugLevel::INFO);
            sig_exit_loop();
        }
    }
    Instrumentor::Get().EndSession();
}



void Orchestrator::shutdown() {
    m_running = false;
    /*Shutoff sequence*/
    m_world_updater.shutdown();
    m_imgui_updater.shutdown();
}




