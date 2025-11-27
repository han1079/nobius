#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/entity.h>

/**
 * EntityManager - Manages entity registration, updates, and deletion
 * Extracted from Renderer to separate concerns
 */
class EntityManager {
public:
    EntityManager() = default;
    ~EntityManager() = default;

    // Entity management interface
    void register_new_entity(const EntityData& entity);
    void update_entity(const EntityData& entity);
    void delete_entity(const EntityData& entity);

    // Access for renderer (const to prevent mutation outside manager)
    const std::unordered_map<uint64_t, EntityMemory>& get_entity_registry() const { return m_entity_registry; }
    const std::vector<unsigned int>& get_index_buffer() const { return m_IBO_mirror; }
    bool is_index_buffer_dirty() const { return m_IBO_dirty; }
    void mark_index_buffer_clean() { m_IBO_dirty = false; }

    // For debugging
    size_t get_entity_count() const { return m_entity_registry.size(); }

private:
    // Entity storage (moved from Renderer)
    std::unordered_map<uint64_t, EntityMemory> m_entity_registry{};
    std::vector<unsigned int> m_IBO_mirror{};
    bool m_IBO_dirty = false;
};