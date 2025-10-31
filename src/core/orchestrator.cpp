#include <app/orchestrator.h>

Orchestrator* Orchestrator::m_orchestrator_ptr = nullptr;



void Orchestrator::init(){
    _ 
}

void Orchestrator::run() {
    /*Main Loop Lives Here*/
    using clock = std::chrono::steady_clock;
    auto last_time = clock::now();
    while (m_running) {
        auto current_time = clock::now();
        std::chrono::duration<float> delta = current_time - last_time;
        last_time = current_time;
        float dT = delta.count();

        // Ingest event buffer from SDL and update state tracker
        m_state_updater::update();

        // Update world data based on state update and dT
        m_world_updater::update(dT);

        // Run renderer with world data
        m_renderer::update();   
    }
}





