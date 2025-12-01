#pragma once
#include <cstdint>
#include <pch.h>
#include <core/entity.h>
#include <set>


using UUID_t = uint64_t;
struct AllocatedMem {
    unsigned int offset = 0;
    unsigned int vertex_count = 0;
    UUID_t uuid = 0;
};

using p_Entity_ro = const Entity*;
using EntityRequest = std::pair<UUID_t, p_Entity_ro>;
using BatchEntityRequest = std::set<EntityRequest>;

using BatchedMemoryMap = std::unordered_map<UUID_t, AllocatedMem>;

class VertexAllocator {
public:
    VertexAllocator();
    ~VertexAllocator();
public:
    // Getters 

    void init();

    bool is_allocated(const UUID_t& uuid) const { return (validate_uuid_exists(uuid)); }
    std::optional<unsigned int> get_allocated_size(const UUID_t& uuid) const;

    bool has_enough_space(const int& vertex_count) const { return (memory_edge + vertex_count <= gpu_vertex_buffer.size()) ? true : false; }
    bool needs_compactification() { return (get_memory_fragmentation_percentage() > FRAGMENTATION_THRESHOLD) ? true : false; }

public:
    // Public API for Allocator 

    BatchedMemoryMap allocate_memory(const BatchEntityRequest& request);
    BatchedMemoryMap reallocate_memory(const BatchEntityRequest& request);
    int memory_free(const std::set<UUID_t>& request);
    BatchedMemoryMap request_memory_offsets(const std::set<UUID_t>& request);
    BatchedMemoryMap update_memory_data(const BatchEntityRequest& request);
    
    // Overloads for single entity cases
    BatchedMemoryMap allocate_memory(const EntityRequest& request);
    BatchedMemoryMap reallocate_memory(const EntityRequest& request);
    int memory_free(const UUID_t& request);
    BatchedMemoryMap request_memory_offsets(const UUID_t& request);
    BatchedMemoryMap update_memory_data(const EntityRequest& request);

    // Used by Renderer to get latest locations for VBO data + the most up-to-date data as read-only.
    std::vector<AllocatedMem> get_dirty_allocations();
    const std::vector<RichVertex>& get_vertex_buffer_data() const { return gpu_vertex_buffer; }

    // Run periodically by Entity Manager if it it sees sufficient fragmentation
    std::optional<BatchedMemoryMap> request_compactification();

    // Run periodically by Entity Manager to check that all entities accounted for. 
    bool validate_full_uuid_match(const std::set<UUID_t>& uuid_list) const;

    bool mark_uuid_dirty(const UUID_t& uuid) {
        if(!validate_uuid_exists(uuid)) {
            return false;
        }
        dirty_entities.insert(uuid);
        return true;
    }

    bool mark_uuid_clean(const UUID_t& uuid) {
        if(!validate_uuid_exists(uuid)) {
            return false;
        }
        dirty_entities.erase(uuid);
        return true;
    }


private:

    std::vector<RichVertex> gpu_vertex_buffer;
    std::unordered_map<UUID_t, AllocatedMem> entity_memory_list;
    std::set<UUID_t> dirty_entities;
    int memory_edge = 0;
    unsigned int dead_memory = 0;
    unsigned int used_memory = 0;

    bool compactification_wanted = false;

private:
    // Validation Functions

    /*Checks vertex count is nonzero, positive, and doesn't overflow the buffer.*/
    bool validate_allocation_size(int vertex_count);

    /*Checks that the UUID in question is available to be allocated, and not already in use*/
    bool validate_uuid_available(const UUID_t& uuid) const;

    /*Checks that the UUID in question has already been allocated, and there's only one record.*/
    bool validate_uuid_exists(const UUID_t& uuid) const;

    /*Comprehensive check on entire entity_memory_list*/ 
    bool health_check();

private:
    // Basic Helper Functions 
    std::optional<AllocatedMem> allocate_memory_for_uuid(const UUID_t& uuid, const int& entity_size);
    std::optional<AllocatedMem> reallocate_memory_for_uuid(const UUID_t& uuid, const int& entity_size);

    /*Not a traditional free. More of a memory leak tracker. Removes the entity from registration list.*/
    std::optional<int> free_memory_for_uuid(const UUID_t& uuid);

    // Skipping complex failure handling for now. TODO
    bool assign_memory(const int& offset, const std::vector<RichVertex>& verts);
    float get_memory_fragmentation_percentage() { return ((dead_memory > 0) ? ((float)memory_edge / (float)dead_memory) : 0.0f); }
    void compactify();

private:
    static constexpr unsigned int MAX_VERTEX_BUFFER_SIZE = 1024*128;
    static constexpr float FRAGMENTATION_THRESHOLD = 0.3f;
};
