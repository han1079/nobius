#pragma once
#include <core/base_updater.h>
#include <pch.h>
#include <core/common.h>
#include <core/entity.h>
#include <core/vertex_allocator.h>
#include <core/entity_manager.h>
#include <random>


class WorldUpdater : public BaseUpdater{
public:
    WorldUpdater() = default;
    ~WorldUpdater() = default;
    friend class Orchestrator;

private:
    // Consolidated world state (was in WorldState)
    glm::mat4 projection_mat = glm::mat4(1.0f);
    glm::mat4 view_mat = glm::mat4(1.0f);
    glm::mat4 clip_transform_mat = glm::mat4(1.0f);
    EntityManager m_entity_manager;

    float time_since_startup = 0.0f;
    float world_units_per_screen_height = 5.0f;
    float get_world_units_per_screen_height() const { return world_units_per_screen_height; }

    float current_zoom = 1.0f;
    float get_current_zoom() const { return current_zoom; }

    glm::vec2 camera_position = glm::vec2(0.0f, 0.0f);
    glm::vec2 get_camera_position() const { return camera_position; }

public:
    // Direct accessors (replace template getters)
    const glm::mat4& get_clip_transform() const { return clip_transform_mat; }
    void set_clip_transform(const glm::mat4& transform) { clip_transform_mat = transform; }
    void add_circle(const glm::vec3& center, float radius, const glm::vec4& color);
    
protected:

    bool init() override;
    bool shutdown() override;
    bool update_state_via_event() override;
    bool update_state_via_dT(float dT) override;
    bool submit_render_commands() override;

};

