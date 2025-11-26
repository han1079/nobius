#pragma once
#include <core/base_updater.h>
#include <pch.h>
#include <core/common.h>
#include <state/world_state.h>
#include <random>

class WorldUpdater : public BaseUpdater{
public:
    WorldUpdater() = delete;
    WorldUpdater(WorldState& state);
    ~WorldUpdater() = default;
    friend class Orchestrator;

private:
    WorldState& m_world_data;
    
protected:

    bool init() override;
    bool shutdown() override;
    bool update_state_via_event(EngineEvent &event) override;
    bool update_state_via_dT(float dT) override;
    uint64_t generate_uuid();

};

