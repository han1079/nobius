#include <core/vertex_allocator.h>

VertexAllocator::VertexAllocator() :
    gpu_vertex_buffer(MAX_VERTEX_BUFFER_SIZE),
    entity_memory_list(),
    dirty_entities() {}

VertexAllocator::~VertexAllocator() {
    // Does nothing for now. Everything is RAII. No pointers are returned.
}

std::optional<unsigned int> VertexAllocator::get_allocated_size(const UUID_t& uuid) const {
    if(entity_memory_list.count(uuid) == 1) {
        return entity_memory_list.at(uuid).vertex_count;
    }
    Debug::log("No Matching UUID found to return size.", DebugLevel::ERROR);
    return std::nullopt;
}

bool VertexAllocator::validate_allocation_size(int vertex_count) {
    if (memory_edge + vertex_count <= gpu_vertex_buffer.size() && vertex_count > 0) {
        return true;
    }
    return false;
}

bool VertexAllocator::validate_uuid_available(const UUID_t& uuid) const {
    if(entity_memory_list.count(uuid) == 0) {
        return true;
    }
    return false;
}

bool VertexAllocator::validate_uuid_exists(const UUID_t& uuid) const {
    if(entity_memory_list.count(uuid) == 1) {
        return true;
    }
    return false;
}


std::optional<AllocatedMem> VertexAllocator::allocate_memory_for_uuid(const UUID_t& uuid, const int& entity_size) {
    if(!validate_uuid_available(uuid)) { return std::nullopt; } 
    if(!validate_allocation_size(entity_size)) { return std::nullopt; }

    AllocatedMem new_mem = {(unsigned)memory_edge, (unsigned)entity_size, uuid};
    memory_edge += entity_size;
    used_memory += entity_size;
    return new_mem;
}

std::optional<AllocatedMem> VertexAllocator::reallocate_memory_for_uuid(const UUID_t& uuid, const int& entity_size) {
    if(!validate_uuid_exists(uuid)) { return std::nullopt; } 
    if(!free_memory_for_uuid(uuid)) { return std::nullopt; }
    std::optional<AllocatedMem> allocated_mem = allocate_memory_for_uuid(uuid, entity_size);
   
    if (allocated_mem.has_value()) {
        return allocated_mem;
    } else {
        return std::nullopt;
    }

}

std::optional<int> VertexAllocator::free_memory_for_uuid(const UUID_t& uuid) {
    if(!validate_uuid_exists(uuid)) { return std::nullopt; }

    int freed_size = entity_memory_list.at(uuid).vertex_count;
    ASSERT(freed_size > 0);

    // Direct deletion from map and set. Without the offset keys - data is unable to be accessed.
    // in the vertex buffer.
    entity_memory_list.erase(uuid);
    dirty_entities.erase(uuid);

    // Zero Change in total memory usage
    dead_memory += freed_size;
    used_memory -= freed_size;
    return freed_size;
}

bool VertexAllocator::assign_memory(const int& offset, const std::vector<RichVertex>& verts) {
    if(!validate_allocation_size(verts.size())) return false;
    
    for (int i = 0; i < verts.size(); i++) {
        gpu_vertex_buffer[offset + i] = verts[i];
    }

    return true;
}

bool VertexAllocator::health_check() {
    bool healthy = false;

    // unordered_map ensures UUID is unique. We just need to check that
    // 1. No overlaps
    // 2. No zero sized allocations
    // 3. No "outside gpu_vertex_buffer" 
    // 4. Memory Edge, Dead Memory, and Used Memory are consistent

    std::vector<AllocatedMem> temp_sorted;
    temp_sorted.reserve(entity_memory_list.size());

    for (auto& [key, val] : entity_memory_list) {
        temp_sorted.emplace_back(val);
    }

    std::sort(temp_sorted.begin(), temp_sorted.end(), [](const AllocatedMem& a, const AllocatedMem& b) {
        return a.offset < b.offset;
    });

    unsigned int previous_end = 0;
    bool zero_detected = false;
    unsigned int total_allocated = 0;

    for (auto& interval : temp_sorted) {
        // Only ONE interval should have offset start at zero.
        if (interval.offset == 0) {
            if (!zero_detected) {
                zero_detected = true;
            } else {
                Debug::log("Multiple Zero Offset Allocs Found!", DebugLevel::ERROR);
                return false;
            }
        }
        
        if (interval.vertex_count == 0) {
            Debug::log("Zero Length Alloc Found!", DebugLevel::ERROR);
            return false;
        }

        if (interval.uuid != entity_memory_list.at(interval.uuid).uuid) {
            Debug::log("Allocated Memory's UUID record does not match key!", DebugLevel::ERROR);
            return false;
        }

        if (previous_end > interval.offset) {
            Debug::log("Overlap detected!", DebugLevel::ERROR);
        }

        previous_end = interval.offset + interval.vertex_count;
        total_allocated += interval.vertex_count;
    }

    if (previous_end != memory_edge) {
        Debug::log("Edge of Memory does not correspond to furthest out edge", DebugLevel::ERROR);
        return false;
    }

    if (total_allocated != used_memory) {
        Debug::log("Allocated memory size and tracked size are inconsistent", DebugLevel::ERROR);
        return false;
    }

    if (previous_end > gpu_vertex_buffer.size()) {
        Debug::log("Allocation extends outside of buffer size", DebugLevel::ERROR);
        return false;
    }

    if ((memory_edge - total_allocated) != dead_memory) {
        Debug::log("Freed Memory Size tracking is inconsistent", DebugLevel::ERROR);
        return false;
    }

    return true;
}

void VertexAllocator::compactify() {
    if(!dirty_entities.empty()) { 
        Debug::log("Memory Should be Stable Before Compactification", DebugLevel::ERROR); 
        return;
    }
    ASSERT((dead_memory + used_memory == memory_edge)); // Basic sanity check that all of the allocs have been consistent
    std::unordered_map<UUID_t, AllocatedMem> tmp_mem = {};
    
    UUID_t curr_uuid = 0;
    unsigned int new_offset = 0;
    unsigned int vcount = 0;
    for (auto& segment : entity_memory_list) {
        curr_uuid = segment.first;
        vcount = segment.second.vertex_count;
        AllocatedMem new_alloc = {new_offset, vcount, curr_uuid};
        tmp_mem.insert({curr_uuid, new_alloc});
        new_offset += vcount;
    }
    memory_edge = new_offset;
    dead_memory = 0;

    ASSERT((used_memory == memory_edge)); // Sanity check that this creates perfect memory usage.
    entity_memory_list.swap(tmp_mem);
}

/*Public API Implementations*/
std::vector<AllocatedMem> VertexAllocator::get_dirty_allocations() {
    if(dirty_entities.empty()) { return {}; }

    std::vector<AllocatedMem> dirty_allocations;
    dirty_allocations.reserve(dirty_entities.size());
    for (auto& uuid : dirty_entities) {
        dirty_allocations.emplace_back(entity_memory_list.at(uuid));
    }

    return dirty_allocations;
}

bool VertexAllocator::validate_full_uuid_match(const std::set<UUID_t>& uuid_list) const {
    bool matching = true;
    for (auto& uuid : uuid_list) {
        if(entity_memory_list.count(uuid) != 1) {
            matching = false;
            break;
        }
    }
    return matching;
}

// Overloads for single entity cases
BatchedMemoryMap VertexAllocator::allocate_memory(const EntityRequest& request) {
    BatchedMemoryMap to_rtn;
    int esize = request.second->get_vertex_size();
    std::vector<RichVertex> verts = request.second->get_vertex_buf_data();
    std::optional<AllocatedMem> alloc_mem = allocate_memory_for_uuid(request.first, esize);
    
    if (alloc_mem.has_value()){
        AllocatedMem tmp = {alloc_mem.value()};  
        to_rtn.insert({request.first, tmp});
        assign_memory(tmp.offset, verts);
        entity_memory_list.insert_or_assign(request.first, tmp);
        dirty_entities.insert(request.first);
    } else {
        ASSERT(false); // Safeguards from smaller functions should prevent this. If it gets here, panic.
    }
    return to_rtn; // Due to assert safeguard - we should always be safe to default return a nonempty batch.
}

BatchedMemoryMap VertexAllocator::reallocate_memory(const EntityRequest& request) {
    BatchedMemoryMap to_rtn;
    int esize = request.second->get_vertex_size();
    std::vector<RichVertex> verts = request.second->get_vertex_buf_data();
    std::optional<AllocatedMem> realloc_mem = reallocate_memory_for_uuid(request.first, esize);

    if (realloc_mem.has_value()){
        AllocatedMem tmp = {realloc_mem.value()};  
        to_rtn.insert({request.first, tmp});
        assign_memory(tmp.offset, verts);
        entity_memory_list.insert_or_assign(request.first, tmp);
        dirty_entities.insert(request.first);
    } else {
        ASSERT(false); // Safeguards from smaller functions should prevent this. If it gets here, panic.
    }
    return to_rtn; // Due to assert safeguard - we should always be safe to default return a nonempty batch.
}

int VertexAllocator::memory_free(const UUID_t& request) {
    std::optional<int> result = free_memory_for_uuid(request);
    if(result.has_value()) { 
        return result.value(); 
    }
    return -1;
}
BatchedMemoryMap VertexAllocator::request_memory_offsets(const UUID_t& request) {
    if (!validate_uuid_exists(request)) { return {}; } // Safe here to just return empty struct, since it's a request.

    BatchedMemoryMap to_rtn;
    to_rtn.insert({request, entity_memory_list.at(request)});
    return to_rtn;
}

BatchedMemoryMap VertexAllocator::allocate_memory(const BatchEntityRequest& request) {

    BatchedMemoryMap to_rtn;
    for (auto& req : request) {
        int esize = req.second->get_vertex_size();
        std::vector<RichVertex> verts = req.second->get_vertex_buf_data();
        std::optional<AllocatedMem> alloc_mem = allocate_memory_for_uuid(req.first, esize);
        if (alloc_mem.has_value()){
            AllocatedMem tmp = {alloc_mem.value()};  
            to_rtn.insert({req.first, tmp});
            assign_memory(tmp.offset, verts);
            entity_memory_list.insert_or_assign(req.first, tmp);
            dirty_entities.insert(req.first);
        } else {
            ASSERT(false); // Safeguards from smaller functions should prevent this. If it gets here, panic.
        }
    }
    return to_rtn; // Due to assert safeguard - we should always be safe to default return a nonempty batch.
}
BatchedMemoryMap VertexAllocator::reallocate_memory(const BatchEntityRequest& request) {

    BatchedMemoryMap to_rtn;
    for (auto& req : request) {
        int esize = req.second->get_vertex_size();
        std::vector<RichVertex> verts = req.second->get_vertex_buf_data();
        std::optional<AllocatedMem> realloc_mem = reallocate_memory_for_uuid(req.first, esize);
        if (realloc_mem.has_value()){
            AllocatedMem tmp = {realloc_mem.value()};  
            to_rtn.insert({req.first, tmp});
            assign_memory(tmp.offset, verts);
            entity_memory_list.insert_or_assign(req.first, tmp);
            dirty_entities.insert(req.first);
        } else {
            ASSERT(false); // Safeguards from smaller functions should prevent this. If it gets here, panic.
        }
    }
    return to_rtn; // Due to assert safeguard - we should always be safe to default return a nonempty batch.
}

int VertexAllocator::memory_free(const std::set<UUID_t>& request) {
    int total_freed = 0;
    
    // Before doing a batch free - we need to verify that ALL UUIDs are valid.
    // Individual safety checks will cause a partial batch erase otherwise!
    for (auto& req : request) {
        if(!validate_uuid_exists(req)){
            return -1;
        }
    }

    for (auto& req : request){
        std::optional<int> result = free_memory_for_uuid(req);

        if(result.has_value() && result.value() != 0) { // Zero case should be caught in above assert, but just careful here.
            total_freed += result.value(); 
        } else {
            ASSERT(false); // Every free should have a valid value by this point!
        }
    }
    return total_freed;
}

BatchedMemoryMap VertexAllocator::request_memory_offsets(const std::set<UUID_t>& request) {
    BatchedMemoryMap to_rtn;
    for (auto& req : request) {
        if(!validate_uuid_exists(req)){ //Exit early if there's an invalid UUID.
            return {};
        }
        to_rtn.insert({req, entity_memory_list.at(req)});
    }
    return to_rtn;
}

std::optional<BatchedMemoryMap> VertexAllocator::request_compactification() {
    if (!needs_compactification()) { return std::nullopt; } // Shouldn't happen since request should happen AFTER check.


    if(!dirty_entities.empty()) { 
        Debug::log("Memory Should be Stable Before Compactification", DebugLevel::ERROR); 
        return std::nullopt;
    }
    
    if(!health_check()) {
        Debug::log("Compactification requested on corrupted memory. Attempting to fix", DebugLevel::INFO);
    }
    
    // Creates temporary objects to hold the compactified memory
    BatchedMemoryMap tmp_mem = {};
    std::vector<RichVertex> tmp_vertex_buffer;
    tmp_vertex_buffer.reserve(gpu_vertex_buffer.size());
    
    UUID_t curr_uuid = 0;
    unsigned int new_offset = 0;
    unsigned int vcount = 0;
    
    for (auto& segment : entity_memory_list) {
        curr_uuid = segment.first;
        vcount = segment.second.vertex_count;

        if (vcount == 0) {
            Debug::log("Zero sized segment found. Not including in compactification.", DebugLevel::INFO);
            continue;
        }

        AllocatedMem new_alloc = {new_offset, vcount, curr_uuid};
        tmp_mem.insert({curr_uuid, new_alloc});

        for (int i = 0; i < vcount; i++) {
            tmp_vertex_buffer[new_offset + i] = gpu_vertex_buffer[segment.second.offset + i];
        }
        new_offset += vcount;
    }
    memory_edge = new_offset;
    used_memory = new_offset;
    dead_memory = 0;

    if(!health_check()) {
        Debug::log("Post Compactification did not fix memory", DebugLevel::FATAL);
        ASSERT(false); //If for some reason FATAL doesn't crash - assert and debug hook here.
    }

    entity_memory_list.swap(tmp_mem);
    gpu_vertex_buffer.swap(tmp_vertex_buffer);

    return entity_memory_list;
}
