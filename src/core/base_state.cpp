#include <core/base_state.h>

// Default constructor
BaseState::BaseState() = default;

// Constructor with file path
BaseState::BaseState(const std::optional<std::string>& fpath) : file_path(fpath) {}

// Destructor - needs to be defined even if = default because it's virtual
BaseState::~BaseState() = default;

// Virtual function implementations
void BaseState::load_from_json(const std::optional<std::string>& fpath) {
    // Default implementation - can be overridden by derived classes
    // TODO: Implement base JSON loading logic
}

void BaseState::write_json_to_file(const std::string& fpath, const nlohmann::json& j) {
    // Default implementation - can be overridden by derived classes
    // TODO: Implement base JSON writing logic
}
