#include <updaters/mode_updater.h>
#include <updaters/orchestrator.h>

ModeUpdater::ModeUpdater(ModeState& state) : m_mode_data(state) {}

bool ModeUpdater::init() {
    return true;
}

bool ModeUpdater::shutdown() {
    return true;
}

bool ModeUpdater::update_state_via_event(SDL_Event &event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT)
        ORCH()->shutdown();
    else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
        ORCH()->shutdown();
    return true;
}

bool ModeUpdater::update_state_via_dT(float dT) {
    return true;
}
