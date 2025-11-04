
#include <state/world_state.h>

WorldState::WorldState(const std::optional<std::string>& fpath) : BaseState(fpath) {
    load_from_json(fpath);
}

void WorldState::load_from_json(const std::optional<std::string>& fpath) {
    // Blank function since there is no config.
    return;
}
