#pragma once
#include <pch.h>
#include <core/entity.h>
#include <core/vertex_allocator.h>
#include <random>

class EntityManager {
public:
    EntityManager() = default;
    ~EntityManager() = default;
    // Methods for entity management would go here

    std::unordered_map<uint64_t, Entity> EntityRegister;
    bool validate_entity_exists(const uint64_t& uuid) const { return (EntityRegister.find(uuid) != EntityRegister.end()); }
    uint64_t generate_and_register_entity();

    BatchedMemoryMap update_gpu_buffer();
    
    void remove_entity(const uint64_t& uuid);
    void translate_entity(const uint64_t& uuid, const glm::vec3& dpos);
    void scale_entity(const uint64_t& uuid, float dsize);
    void rotate_entity(const uint64_t& uuid, float drot);
    void set_entity_position(const uint64_t& uuid, const glm::vec3& pos); 
    void set_entity_size(const uint64_t& uuid, float size);
    void set_entity_rotation(const uint64_t& uuid, float rot);

public:

    void configure_entity_as_type(const uint64_t& uuid, EntityType type); 
    void run_vertex_transform(Entity& entity);

public:

    uint64_t generate_circle(float radius, const glm::vec3& center);    

private:

    uint64_t generate_uuid();
    std::set<uint64_t> dirty_uuids;


};