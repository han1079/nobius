#include <core/entity_manager.h>
#include <core/orchestrator.h>
#include <engines/geometry.h>

uint64_t EntityManager::generate_and_register_entity() {
    uint64_t uuid = generate_uuid();
    if (validate_entity_exists(uuid)) {
        return false; // Entity already exists
    }
    Entity entity(uuid);
    EntityRegister.try_emplace(uuid, std::move(entity));
    return uuid;
}

void EntityManager::run_vertex_transform(Entity& entity) {
    switch (entity.get_type()) {
        case EntityType::CIRCLE:
            Geometry::Affine::TransformCircle(entity);
            break;
        // Add cases for other entity types as needed
        default:
            break;
    }
}

void EntityManager::configure_entity_as_type(const uint64_t& uuid, EntityType type) {
    auto it = EntityRegister.find(uuid);
    if (it != EntityRegister.end()) {
        it->second.set_type(type);
    }

    if (type == EntityType::CIRCLE) {
        Geometry::Build::CreateCircle(it->second);
    }
}

void EntityManager::remove_entity(const uint64_t& uuid) {
    if(validate_entity_exists(uuid)) { 
        EntityRegister.erase(uuid); 
        auto& va = Orchestrator::get()->get_vertex_allocator();
        va.memory_free(uuid);
    } else { 
        Debug::log("Attempted to remove non-existent entity UUID: " +  std::to_string(uuid), DebugLevel::ERROR); 
    } 
}

 void EntityManager::translate_entity(const uint64_t& uuid, const glm::vec3& dpos) {
        auto it = EntityRegister.find(uuid);
        if (it != EntityRegister.end()) {
            it->second.pose.update_dpos(dpos);
        }
    }

void EntityManager::scale_entity(const uint64_t& uuid, float dsize) {
    auto it = EntityRegister.find(uuid);
    if (it != EntityRegister.end()) {
        it->second.pose.update_dsize(dsize);
    }
}

void EntityManager::rotate_entity(const uint64_t& uuid, float drot) {
    auto it = EntityRegister.find(uuid);
    if (it != EntityRegister.end()) {
        it->second.pose.update_drot(drot);
    }
}

void EntityManager::set_entity_position(const uint64_t& uuid, const glm::vec3& pos) {
    auto it = EntityRegister.find(uuid);
    if (it != EntityRegister.end()) {
        it->second.pose.set_position(pos);
    }
}

void EntityManager::set_entity_size(const uint64_t& uuid, float size) {
    auto it = EntityRegister.find(uuid);
    if (it != EntityRegister.end()) {
        it->second.pose.set_size(size);
    }
}

void EntityManager::set_entity_rotation(const uint64_t& uuid, float rot) {
    auto it = EntityRegister.find(uuid);
    if (it != EntityRegister.end()) {
        it->second.pose.set_rotation(rot);
    }
}


BatchedMemoryMap EntityManager::update_gpu_buffer()
{
    auto& va = Orchestrator::get()->get_vertex_allocator();
    BatchedMemoryMap map;
    dirty_uuids.clear();
    for (auto& [uuid, entity] : EntityRegister) {
        if (entity.pose.transform_dirty) {
            run_vertex_transform(entity);
            dirty_uuids.insert(uuid);
        }

        map = va.update_memory_data({{uuid, &entity}});
    }
    Debug::log("Dirty UUIDs updated: " + std::to_string(dirty_uuids.size()), DebugLevel::TRACE);
    return map;
}

uint64_t EntityManager::generate_circle(float radius, const glm::vec3& center) {
    uint64_t uuid = generate_and_register_entity();
    Entity& circle_entity = EntityRegister.at(uuid);

    configure_entity_as_type(uuid, EntityType::CIRCLE);
    circle_entity.pose.set_position(center);
    circle_entity.pose.set_size(radius);

    auto& va = Orchestrator::get()->get_vertex_allocator();
    va.allocate_memory({{uuid, &circle_entity}});

    return uuid;
}