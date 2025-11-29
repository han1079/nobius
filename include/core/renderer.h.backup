#pragma once
#include <pch.h>
#include <core/common.h>
#include <core/entity.h>
#include <state/imgui_state.h>
#include <random>
#include <cstdint>
#include <queue>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>



// Vertex attribute constants
struct RichVertexAttr {
    static constexpr int BOUNDINGBOX_LOC = 0;
    static constexpr int STARTPT_LOC = 1;
    static constexpr int ENDPT_LOC = 2;
    static constexpr int CONTROLPT1_LOC = 3;
    static constexpr int CONTROLPT2_LOC = 4;
    static constexpr int TEXCOORDS_LOC = 5;
    static constexpr int THICKNESS_LOC = 6;
    static constexpr int FILLED_LOC = 7;

    static constexpr int TOTAL_ATTRS = 8;
    static constexpr int STRIDE = sizeof(RichVertex) / sizeof(float);

    static constexpr int BOUNDINGBOX_SIZE = 3;
    static constexpr int STARTPT_SIZE = 3;
    static constexpr int ENDPT_SIZE = 3;
    static constexpr int CONTROLPT1_SIZE = 3;  
    static constexpr int CONTROLPT2_SIZE = 3;
    static constexpr int TEXCOORDS_SIZE = 3;
    static constexpr int THICKNESS_SIZE = 1;
    static constexpr int FILLED_SIZE = 1;

    static constexpr int BOUNDINGBOX_OFFSET = 0;
    static constexpr int STARTPT_OFFSET = BOUNDINGBOX_OFFSET + BOUNDINGBOX_SIZE;
    static constexpr int ENDPT_OFFSET = STARTPT_OFFSET + STARTPT_SIZE;
    static constexpr int CONTROLPT1_OFFSET = ENDPT_OFFSET + ENDPT_SIZE;  
    static constexpr int CONTROLPT2_OFFSET = CONTROLPT1_OFFSET + CONTROLPT1_SIZE;
    static constexpr int TEXCOORDS_OFFSET = CONTROLPT2_OFFSET + CONTROLPT2_SIZE;
    static constexpr int THICKNESS_OFFSET = TEXCOORDS_OFFSET + TEXCOORDS_SIZE;
    static constexpr int FILLED_OFFSET = THICKNESS_OFFSET + THICKNESS_SIZE;
};

enum class alloc_tags {
    INIT, ACK, ALLOC, FREE, SHRINK, GROW, MISMATCH
};

struct FragmentInterval {
    unsigned int start;
    unsigned int size;
    alloc_tags status = alloc_tags::INIT;
};

struct EntityMemory {
    std::vector<FragmentInterval> intervals;
    unsigned int total_size = 0;
    unsigned int first_start = 0;
    unsigned int last_start = 0;
    unsigned int total_intervals = 0;

    bool visible = true;
    std::vector<int> cached_ibo_segment = {};
    
    struct ingested_flags {
        bool to_delete = false;
        std::optional<bool> next_vis_state = std::nullopt;
        bool size_changed = false;
        bool verts_changed = false;
        int first_seen_idx = -1;
        bool new_entity = false;
    } ingested_flags;

    void reset_flags() { ingested_flags = {}; }
};

class MemoryList {
public:
    struct MemoryFragment {
        unsigned int start = 0;
        unsigned int size = 0;
        bool free = true;
        MemoryFragment* next = nullptr;
        MemoryFragment* prev = nullptr;
    };

    struct MemoryIterator {
        MemoryIterator(MemoryFragment* vf, MemoryList* o) : viewed_fragment(vf), owner(o) {}
        
        MemoryFragment* viewed_fragment;
        MemoryList* owner;

        MemoryIterator& operator++();
        MemoryIterator operator++(int);
        MemoryIterator& operator--();
        MemoryIterator operator--(int);
        MemoryFragment& operator*() const { return *viewed_fragment; }
        MemoryFragment* operator->() const { return viewed_fragment; }
        bool operator!=(const MemoryIterator& other) { return (viewed_fragment != other.viewed_fragment); }
        bool operator==(const MemoryIterator& other) { return (viewed_fragment == other.viewed_fragment); }
    };

    explicit MemoryList(unsigned int total_size);
    ~MemoryList();

    // The only variables that this class owns.
    MemoryFragment* head;
    MemoryFragment* tail;

    // Iterator functions
    MemoryIterator begin() { return MemoryIterator(head, this); }
    MemoryIterator end() { return MemoryIterator(nullptr, this); }
    MemoryIterator iterate_from(MemoryFragment* n) { return MemoryIterator(n, this); }

    // Linked list / memory allocator specific functions
    void clear_fragments();
    [[nodiscard]] bool adjacency_check(MemoryFragment* prev, MemoryFragment* next);
    bool allocate_into(MemoryFragment* target_fragment, unsigned int requested_size);
    bool free_block(MemoryFragment* target_fragment);
    bool shrink_allocated_block(MemoryFragment* target_fragment, unsigned int new_size);
    bool grow_allocated_block(MemoryFragment* target_fragment, unsigned int new_size);
    bool coalesce(MemoryFragment* target_fragment);
};

class MemoryManager {
public:
    struct MemoryStats {
        unsigned int n;
        unsigned int size;
        float mu;
        float M2;

        void add(unsigned int d_size);
        void remove(unsigned int d_size);
        float get_variance();
        float get_entropy();
        bool is_empty();
    };

    MemoryManager(unsigned int size);
    ~MemoryManager() = default;

    unsigned int total_size;
    MemoryList memory_list;
    std::vector<RichVertex> gpu_memory_heap;
    MemoryStats alloc_stats;
    MemoryStats free_stats;

    void write_vertex_to_heap_idx(unsigned int heap_index, const RichVertex& vertex);

    using MemoryFragment = MemoryList::MemoryFragment;

    void free_memory(EntityMemory& container, int size = -1);
    void resize_memory(EntityMemory& container, unsigned int new_size);
    void allocate_memory(unsigned int num, EntityMemory& container);
    void allocate_using_append(unsigned int num, EntityMemory& container);
    void allocate_using_walk(unsigned int num, EntityMemory& container);
    float evaluate_alloc_strategy(int proposed_memory_delta);
    bool managed_alloc(MemoryFragment* target, unsigned int size);
    bool managed_grow(MemoryFragment* target, unsigned int new_size);
    bool managed_shrink(MemoryFragment* target, unsigned int new_size);
    bool managed_free(MemoryFragment* target);

private:
    float ENTROPY_THRESHOLD = 1.0;
    static FragmentInterval get_memory_interval(const MemoryList::MemoryFragment& fragment);
    MemoryList::MemoryFragment* walk_to(unsigned int target);
    MemoryList::MemoryFragment* start_from_walk_to(MemoryList::MemoryFragment* init, unsigned int target, bool forward = true);
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    // OpenGL resource IDs
    unsigned int m_VAO_ID{};
    unsigned int m_VBO_ID{};
    unsigned int m_IBO_ID{};

    // Entity management
    std::unordered_map<uint64_t, EntityMemory> m_entity_registry{};
    std::vector<unsigned int> m_IBO_mirror{};
    bool m_IBO_dirty = false;
    MemoryManager memory_manager;

    // Public interface
    bool init();
    void start_frame();
    void render_frame();
    void end_frame();
    void cleanup();

    // Entity management
    void register_new_entity(const EntityData& entity);
    void update_entity(const EntityData& entity);
    void delete_entity(const EntityData& entity);

private:
    void set_up_gpu_buffers();
    void process_request_buffer();
    int acknowledge_memory_allocation(const EntityData& entity);
    int get_offset_index(int local_vertex_idx, std::vector<FragmentInterval> intervals);
};