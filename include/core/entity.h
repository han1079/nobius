#pragma once
#include <pch.h>
#include <core/common.h>
#include <queue>


class RichVertex {
public:
    RichVertex() = default;
    ~RichVertex() = default;

public:

    // Context data for the vertex 
    glm::vec3 bound = {0.0f, 0.0f, 0.0f}; // Use this to draw the quad that contains the curve (this is the actual vertex)
    glm::vec3 start_pt = {0.0f, 0.0f, 0.0f}; // Starting point of the curve. Used by the shader to compute curve
    glm::vec3 end_pt = {0.0f, 0.0f, 0.0f}; // Ending point of the curve. Used by the shader to compute curve
    glm::vec3 control_pt_1 = {0.0f, 0.0f, 0.0f}; // Control point 1 for Bezier curve. Also stands for radius if drawing a circle
    glm::vec3 control_pt_2 = {0.0f, 0.0f, 0.0f}; // Secondary Control Point. Usually dead space, or extra stuff. 

    // Color, thickness, etc

    glm::vec2 tex_coords = {0.0f, 0.0f};
    float thickness = 0;
    int filled = 0;

};
class Entity; // Forward declaration


enum class EntityType {
    CIRCLE,
    BEZIER_CURVE,
    POLYLINE,
    RECTANGLE,
    IMAGE,
    TEXT,
};

class Entity {
public:
    Entity() = delete;
    Entity(uint64_t uuid) : m_uuid(uuid) {}
    
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    ~Entity() = default;

public:
    // Getters
    uint64_t get_uuid() const { return m_uuid; }
    const std::vector<RichVertex>& get_vertex_data() const { return m_vertex_data; }
    const std::vector<RichVertex>& get_base_vertex_data() const { return m_base_vertex_data; }
    const std::vector<int>& get_indices() const { return m_indices; }
    size_t get_vertex_count() const { return m_vertex_data.size(); }
    
    glm::vec4 get_color() const { return m_color; }
    EntityType get_type() const { return m_type; }

    // Setters
    void set_vertex_data(const std::vector<RichVertex>& vertices) { m_vertex_data = vertices; }
    void set_base_vertex_data(const std::vector<RichVertex>& vertices) { m_base_vertex_data = vertices; }
    void set_indices(const std::vector<int>& indices) { m_indices = indices; }
    void set_color(const glm::vec4& color) { m_color = color; }
    void set_type(EntityType type) { m_type = type; }

    // State Setters
    void set_visible(bool visible) { m_visible = visible; }
    bool is_visible() const { return m_visible; }

    void set_selectable(bool selectable) { m_selectable = selectable; }
    bool is_selectable() const { return m_selectable; }

    void set_draggable(bool draggable) { m_draggable = draggable; }
    bool is_draggable() const { return m_draggable; }

    void set_movable(bool movable) { m_movable = movable; }
    bool is_movable() const { return m_movable; }

    struct Pose {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        float size = 1.0f;
        float rotation = 0.0f; // In radians

        glm::mat4 cached_transform = glm::mat4(1.0f);
        bool transform_dirty = true;

        void set_position(const glm::vec3& pos) { position = pos; mark_dirty(); }

        void set_size(float new_size) { size = new_size; mark_dirty(); }

        void set_rotation(float new_rotation) { rotation = new_rotation; mark_dirty(); }

        void update_dpos(const glm::vec3& dp) { position += dp; mark_dirty(); }

        void update_dsize(float ds) { size += ds; mark_dirty(); }

        void update_drot(float dr) { rotation += dr; mark_dirty(); }

        void mark_dirty() { transform_dirty = true; }
        void mark_clean() { transform_dirty = false; }

        glm::mat4 update_transform_matrix(); 
    };
    Pose pose;

private:
    // Identity
    uint64_t m_uuid;

    // Geometry Data
    std::vector<RichVertex> m_vertex_data;
    std::vector<RichVertex> m_base_vertex_data;

    std::vector<int> m_indices;

    // Properties
    EntityType m_type = EntityType::CIRCLE;
    glm::vec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f};

    // State
    bool m_selectable = false;
    bool m_draggable = false;
    bool m_movable = false;
    bool m_visible = true;

    friend class WorldData;
    friend class WorldUpdater;
    friend class Renderer;
};


