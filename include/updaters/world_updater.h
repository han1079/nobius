#pragma once
#include <core/base_updater.h>
#include <pch.h>
#include <core/common.h>
#include <core/entity.h>
#include <random>

class WorldUpdater : public BaseUpdater{
public:
    WorldUpdater() = default;
    ~WorldUpdater() = default;
    friend class Orchestrator;

private:
    // Consolidated world state (was in WorldState)
    int num_entities = 0;
    glm::mat4 clip_transform_mat = glm::mat4(1.0f);
    std::unique_ptr<std::unordered_map<int, Entity>> EntityRegister = nullptr;

public:
    // Direct accessors (replace template getters)
    int get_entity_count() const { return num_entities; }
    const glm::mat4& get_clip_transform() const { return clip_transform_mat; }
    void set_entity_count(int count) { num_entities = count; }
    void set_clip_transform(const glm::mat4& transform) { clip_transform_mat = transform; }
    
protected:

    bool init() override;
    bool shutdown() override;
    bool update_state_via_event(EngineEvent &event) override;
    bool update_state_via_dT(float dT) override;
    uint64_t generate_uuid();

};

