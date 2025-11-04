
#include <state/mode_state.h>

ModeState::ModeState(const std::optional<std::string>& fpath) : BaseState(fpath) {
    load_from_json(fpath);
}

void ModeState::load_from_json(const std::optional<std::string>& fpath) {
    // Blank function since there is no config.
    return;
}
