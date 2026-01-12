#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/entity_manager.h>
#include <core/aggregate.h>

class AggregateManager {
public:
    AggregateManager() = default;
    ~AggregateManager() = default;


    bool validate_aggregate_exists(const UUID_t& uuid) const { return (m_aggregates_register.find(uuid) != m_aggregates_register.end()); }
    UUID_t generate_and_register_aggregate(const EntityManager& entity_manager); // Updates bidirectional maps. Returns UUID.
    void remove_aggregate(const UUID_t& uuid);

    Aggregate& get_aggregate(const UUID_t& uuid);
    void delete_aggregate(const UUID_t& uuid, const EntityManager& entity_manager);

    UUID_t lookup_aggregate_id_for_entity(const UUID_t& entity_uuid, const EntityManager& entity_manager) const;

private:

    std::unordered_map<UUID_t, Aggregate> m_aggregates_register;
    std::unordered_map<UUID_t, UUID_t> m_entity_to_aggregate;
};

