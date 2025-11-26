#pragma once
#include <state/mode_state.h>
#include <core/base_updater.h>
#include <core/common.h>
#include <pch.h>

class ModeUpdater : public BaseUpdater{
public:

    ModeUpdater() = delete;
    ModeUpdater(ModeState& state); 
    ~ModeUpdater() = default;
    friend class Orchestrator;
private:

    ModeState& m_mode_data;

protected:

    bool init() override;
    bool shutdown() override;
    bool update_state_via_event(EngineEvent &event) override;
    bool update_state_via_dT(float dT) override;

};
