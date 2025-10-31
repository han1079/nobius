#pragma once
#include <core/base_updater.h>

#include <core/common.h>
#include <state/world_state.h>

class WorldUpdater : public BaseUpdater{
private:
    WorldState m_world_data;
    

public:
    WorldUpdater() = default;
    ~WorldUpdater() = default;
};

