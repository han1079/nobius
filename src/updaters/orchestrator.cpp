#include <updaters/orchestrator.h>
#include <core/common.h>
#include <iostream>

/*Load Spec Constructor. Member objects find and build with load spec filename string*/
Orchestrator::Orchestrator(const LoadSpec& load_spec):
        m_vertex_allocator(),
        m_input_system(),
        m_renderer(),
        m_world_updater(),
        m_imgui_updater() {
        m_orchestrator_ptr = this;
    }

// Destructor implementation
Orchestrator::~Orchestrator() = default;

void Orchestrator::init(){
    m_vertex_allocator.init();
    // Initialize renderer first to create Window/Context
    m_renderer.init(); 

    // Recursive Init
    m_world_updater.init();
    m_imgui_updater.init();
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

        }

        m_input_system.save_accumulated_changes();
        m_input_system.update_mode();

        m_world_updater.update_state_via_event();
        m_imgui_updater.update_state_via_event();

        loop_event_ct = 0;
        
        m_world_updater.update_state_via_dT(dT);
        m_imgui_updater.update_state_via_dT(dT);

        m_imgui_updater.submit_render_commands();
        m_world_updater.submit_render_commands();

        m_renderer.render();

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




