#include <core/entity_manager.h>
#include <core/common.h>

void EntityManager::register_new_entity(const EntityData& entity) {
    // Implementation moved from Renderer - will be populated next step
    // For now, just add to registry to maintain compilation
    m_entity_registry[entity.UUID] = EntityMemory{};
    m_IBO_dirty = true;
}

void EntityManager::update_entity(const EntityData& entity) {
    // Implementation moved from Renderer - will be populated next step
    if (m_entity_registry.find(entity.UUID) != m_entity_registry.end()) {
        // Update existing entity
        m_IBO_dirty = true;
    }
}

void EntityManager::delete_entity(const EntityData& entity) {
    // Implementation moved from Renderer - will be populated next step
    auto it = m_entity_registry.find(entity.UUID);
    if (it != m_entity_registry.end()) {
        m_entity_registry.erase(it);
        m_IBO_dirty = true;
    }
}