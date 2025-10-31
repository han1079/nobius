#pragma once



#define ORCH() Orchestrator::get()

enum USER_MODES {
    MODE_SELECT = (1 << 0), // Select Mode. Objects hover, and can be clicked on
    MODE_DRAG = (1 << 1), // Drag Mode. Entity is currently being selected and edited
    MODE_MOVE = (1 << 2), // Move Mode. Entity is being moved via center.
    MODE_PLACE = (1 << 3), // Place Mode. New Entity is being placed. Preview is shown.
    MODE_DELETE = (1 << 4), // Delete Mode. Entities can be clicked to delete.
    MODE_INACTIVE = (1 << 5), // No Viewport Interaction Logic apart from "Reactivate"
    MODE_UIFOCUS = (1 << 6) // UI is being interacted with - runs off ImGui IO flag
};
class Orchestrator;
class Renderer;
class WorldUpdater;
class StateUpdater;
class WorldState;
class SystemState;
class WindoState;