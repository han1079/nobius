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
    glm::vec3 bounding_box; // Use this to draw the quad that contains the curve (this is the actual vertex)
    glm::vec3 start_pt; // Starting point of the curve. Used by the shader to compute curve
    glm::vec3 end_pt; // Ending point of the curve. Used by the shader to compute curve
    glm::vec3 control_pt_1; // Control point 1 for Bezier curve. Also stands for radius if drawing a circle
    glm::vec3 control_pt_2; // Control point 2 for Bezier curve

    // Color, thickness, etc

    glm::vec2 tex_coords;
    int thickness;
    int filled;

};

enum BufferRequestType{ 
    REGISTER,
    SHOW,
    HIDE,
    UPDATE,
    DELETE,
};


/** This is a wrapper class that "extracts and holds" entity data.
 * 
 * This way, the Renderer class can manage the GPU-side data without needing
 * to know the details of the Entity structure.
 * 
 * The Entity Structure holds the main copy of this data, mutates,
 * and the Renderer class receives a pointer to it and is able to
 * "give" it the information about memory allocation.
 * 
 * Its identifier is a reference to the UUID in the Entity class.
 * This UUID is linked in at construction time by the Entity class.
 * The Renderer class can then use this UUID to map the EntityData
 * to the Entity that owns it.
*/
class EntityData {
public:
    EntityData() = default;
    ~EntityData() = default;
private:
    std::vector<int> vertex_draw_orders;
    unsigned int vertex_count;
    std::vector<RichVertex> vertex_data;

    const uint64_t& UUID;
 
    friend class Renderer;
    friend class Entity;
};

struct BufferRequest; //Forward Decclaration. TODO: Move this into a proper header.

class Entity {
public:
    uint64_t get_uuid() const {
        return gpu_data.UUID;
    }

    inline static std::queue<BufferRequest> RenderRequestBuffer;
private:
    glm::vec3 origin;
    float size;

    const uint64_t UUID;

    bool selectable;
    bool draggable;
    bool movable;
    bool visible;

    EntityData gpu_data;

    friend class WorldData;
    friend class WorldUpdater;

private:
    void update_gpu_draw_data(const EntityData& new_data) {
        return;
    }
};

struct BufferRequest {
    BufferRequestType type;
    EntityData* p_entity_data;
};

