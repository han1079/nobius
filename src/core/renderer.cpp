#include <pch.h>
#include <core/entity.h>
#include <state/imgui_state.h>
#include <core/renderer.h>
#include <random>
#include <cstdint>
#include <queue>
#include <algorithm>
#include <updaters/orchestrator.h>



const struct RichVertexAttr{
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

        MemoryIterator& operator++() {
            if (viewed_fragment == nullptr){
                viewed_fragment = owner->head;
            } else {

                viewed_fragment = viewed_fragment->next;

                if (viewed_fragment == nullptr) {
                    Debug::log("Reached the end of the memory list.", DebugLevel::TRACE);
                }
            }
            return *this;
        }

        MemoryIterator& operator++(int) {
            MemoryIterator tmp = *this;
            ++(*this);
            return tmp;
        }
            
        MemoryIterator& operator--() {
            if (viewed_fragment == nullptr){
                viewed_fragment = owner->tail; 
            } else {

                viewed_fragment = viewed_fragment->prev;

                if (viewed_fragment == nullptr) {
                    Debug::log("Reached the start of the memory list.", DebugLevel::TRACE);
                }

            }
            return *this;
        }

        MemoryIterator& operator--(int){
            MemoryIterator tmp = *this;
            --(*this);
            return tmp;
        }

        MemoryFragment& operator*() const {
            return *viewed_fragment;
        }

        MemoryFragment* operator->() const {
            return viewed_fragment;
        }

        bool operator!=(const MemoryIterator& other) { return (viewed_fragment != other.viewed_fragment); }
        bool operator==(const MemoryIterator& other) { return (viewed_fragment == other.viewed_fragment); }

    };

    explicit MemoryList(unsigned int total_size)  {
        MemoryFragment* tmp = new MemoryFragment{ 0, total_size, true, nullptr, nullptr };
        head = tmp;
        tail = tmp;
    }

    ~MemoryList() {
        clear_fragments();
    }

public:
    // The only variables that this class owns.
    MemoryFragment* head;
    MemoryFragment* tail;

public:
    // Iterator functions - not functionality unique to this linked list. 
    MemoryIterator begin() { return MemoryIterator(head, this); }
    MemoryIterator end() { return MemoryIterator(nullptr, this); }
    MemoryIterator iterate_from(MemoryFragment* n) { return MemoryIterator(n, this); }

public:
    // Linked list / memory allocator specific functions.
    void clear_fragments(){
        // Iterates through and explicitly deletes all Memory Fragments in the linked list tied to head
        auto* to_delete = head;

        while(to_delete != nullptr) {
            auto* next_node = to_delete->next;
            delete to_delete;
            to_delete = next_node;
        }

        head = nullptr;
        tail = nullptr;
    }

    [[nodiscard]] bool adjacency_check(MemoryFragment* prev, MemoryFragment* next) {
        bool error_detected = false;
        if (!prev || !next) {
            Debug::log("[(ADJ) NULL PTR]. Function not for endpoint checks. Returning.", DebugLevel::ERROR);
            return false;
        }

        if (prev == next) {
            Debug::log("[(ADJ) SAME PTR]. Improper usage. Returning.", DebugLevel::ERROR);
            return false;
        }

        if (prev->next != next) {
            Debug::log("[(ADJ) LINK] Prev does not point to next", DebugLevel::ERROR);
            error_detected = true;
        }

        if (next->prev != prev) {
            Debug::log("[(ADJ) LINK] Next does not point to prev", DebugLevel::ERROR);
            error_detected = true;
        }

        unsigned int expected_n_start = prev->start + prev->size;

        if (expected_n_start > next->start) {
            Debug::log("[(ADJ) SIZE] Prev bleeds into next", DebugLevel::ERROR);
            error_detected = true;
        }

        if (expected_n_start < next->start) {
            Debug::log("[(ADJ) SIZE] Prev does not reach next", DebugLevel::ERROR);
            error_detected = true;
        }

        return (!error_detected);
    }

    bool allocate_into(MemoryFragment* target_fragment, unsigned int requested_size){
        if (!target_fragment) {
            Debug::log("Cannot allocate to nullptr", DebugLevel::ERROR);
            return false;
        }

        if (target_fragment->free != true) {
            Debug::log("Not available to allocate", DebugLevel::ERROR);
            return false;
        }

        if (requested_size == 0) {
            Debug::log("Requested size 0", DebugLevel::ERROR);
            return false;
        }

        if (requested_size > target_fragment->size) {
            Debug::log("Requested size greater than available size.", DebugLevel::ERROR);
            return false;
        }

        unsigned int remainder = target_fragment->size - requested_size;
        target_fragment->size = requested_size;
        target_fragment->free = false;

        if (remainder == 0) {
            return true;
        } 

        // "New" free chunk needs to be created.
        MemoryFragment* free_tail = new MemoryFragment;

        free_tail->start = target_fragment->start + requested_size;
        free_tail->size = remainder;
        free_tail->free = true;


        free_tail->next = target_fragment->next;
        free_tail->prev = target_fragment;

        if (target_fragment->next) {
            // There is a "next segment" - do the usual linking.
            target_fragment->next->prev = free_tail;
        } else {
            // If next fragment is null, we're the end of the list. 
            // The target fragment must be the current tail and there's 
            // no need to assign target->next->prev.
            ASSERT(target_fragment==tail);
            tail = free_tail;
        }

        target_fragment->next = free_tail;

        coalesce(free_tail);

        return true;            
    }

    bool free_block(MemoryFragment* target_fragment){
        if (!target_fragment) {
            Debug::log("Cannot free a nullptr block.", DebugLevel::ERROR);
            return false;
        }

        if (target_fragment->free) {
            Debug::log("Block already freed", DebugLevel::ERROR);
            return false;
        }
        
        target_fragment->free = true;

        coalesce(target_fragment);

        return true;
    }

    bool shrink_allocated_block(MemoryFragment* target_fragment, unsigned int new_size){
        if (!target_fragment) {
            Debug::log("Cannot shrink a nullptr", DebugLevel::ERROR);
            return false;
        }

        if (target_fragment->free) {
            Debug::log("Block not allocated", DebugLevel::ERROR);
            return false;
        }

        if (new_size == 0){
            Debug::log("Cannot shrink to size zero. Please call free instead", DebugLevel::ERROR);
            return false;
        }

        if (new_size > target_fragment->size){
            Debug::log("Cannot shrink to a size greater than current size. Check calcs.", DebugLevel::ERROR);
            return false;
        }

        unsigned int extra_space = target_fragment->size - new_size; 

        if (extra_space == 0) {
            Debug::log("No-op. Requested shrink size and current size are the same.", DebugLevel::INFO);
            return true; 
        }
        target_fragment->size = new_size;

        MemoryFragment* next = target_fragment->next;
        if (next != nullptr && next->free) {
            // The "next" fragment is a valid free block. Adjust size and return.
            next->start = target_fragment->start + new_size;
            next->size += extra_space;
            coalesce(next);
            return true;
        }
            
        // Final case: next fragment is allocated.
        // Shrinking has created a free block.
        // "New" free chunk needs to be created.
        MemoryFragment* free_tail = new MemoryFragment;

        free_tail->start = target_fragment->start + new_size;
        free_tail->size = extra_space;
        free_tail->free = true;


        free_tail->next = next;
        free_tail->prev = target_fragment;

        if (next) {
            // There is a "next segment" - do the usual linking.
            next->prev = free_tail;
        } else {
            // If next fragment is null, we're the end of the list. 
            // The target fragment must be the current tail and there's 
            // no need to assign target->next->prev.
            ASSERT(target_fragment==tail);
            tail = free_tail;
        }

        target_fragment->next = free_tail;

        coalesce(free_tail);

        return true;            
    }


    bool grow_allocated_block(MemoryFragment* target_fragment, unsigned int new_size){

        /*TO_DELETE:
         *
         * Existence Checks: 
         *  targeted block is not null
         *  targeted block is already allocated
         *  target size is not zero 
         *
         *  next block is not null
         *  next block is free
         *  
         * Size Checks:
         *  Requested size is greater than current size
         *  Extra size is available in next free block
         *
         * Cases:
         *  Requested size leaves zero free space
         *  Requested size leaves some free space 
         * */
        if (!target_fragment) {
            Debug::log("Cannot grow a nullptr", DebugLevel::ERROR);
            return false;
        }
        
        if (target_fragment->free) {
            Debug::log("Block not allocated", DebugLevel::ERROR);
            return false;
        }

        if (new_size == 0) {
            Debug::log("Cannot grow to size zero. Please call free instead", DebugLevel::ERROR);
            return false; 
        }

        MemoryFragment* next_fragment = target_fragment->next;
        const unsigned int current_target_size = target_fragment->size;

        if (!next_fragment) {
            Debug::log("At tail. No room to grow.", DebugLevel::ERROR);
            return false;
        }

        if (!next_fragment->free) {
            Debug::log("Next fragment not free", DebugLevel::ERROR);
            return false;
        }

        if (current_target_size > new_size) {
            Debug::log("Cannot grow to a size smaller than current size. Check calcs.", DebugLevel::ERROR);
            return false;
        }

        if (current_target_size == new_size) {
            Debug::log("No-op. Requested growth size is the same as current size.", DebugLevel::INFO);
            return true;
        }

        unsigned int extra_free_space_req = new_size - current_target_size;

        if (next_fragment->size < extra_free_space_req) {
            Debug::log("Free segment not sufficient size.", DebugLevel::ERROR);
            return false;
        }

        unsigned int free_size_after_grow = next_fragment->size - extra_free_space_req;

        target_fragment->size = new_size;
        
        if (free_size_after_grow > 0) {
            
            next_fragment->start = target_fragment->start + new_size;
            next_fragment->size = free_size_after_grow;
            coalesce(next_fragment);

        } else if (free_size_after_grow == 0) {
            MemoryFragment* nn = next_fragment->next;
            
            target_fragment->next = nn;

            if (nn) {
                nn->prev = target_fragment;
            } else {
                ASSERT(next_fragment == tail && target_fragment->next == nullptr);
                tail = target_fragment;
            }

            delete next_fragment;
            next_fragment = nullptr;
        }
        return true;

    }

    bool coalesce(MemoryFragment* target_fragment) {
        /*TO_DELETE:
         *
         * Existence Checks: 
         *  targeted block is not null
         *  targeted block is free 
         *
         * Early Exits:
         *  Both Sides are allocated
         *  Both Sides are null
         *  Neither side has 2 consecutive free;
         *
         * Cases:
         *  Coalesce Right -> Alloc'd, Null, Free
         *  Coalesce Left -> Alloc'd, Null, Free
         *  * */

        if (!target_fragment) {
            Debug::log("Cannot coalesce a nullptr", DebugLevel::ERROR);
            return false;
        }
        
        if (!target_fragment->free) {
            Debug::log("Block not free", DebugLevel::ERROR);
            return false;
        }

        MemoryFragment* target_p = target_fragment->prev;
        MemoryFragment* target_n = target_fragment->next;

        if (!target_p && !target_n) {
            ASSERT(target_fragment == head && target_fragment == tail);
            Debug::log("Only one fragment in list.", DebugLevel::INFO);
            return true;
        }



        bool pfree = false;
        bool nfree = false;

        if (target_p) {
            pfree = target_p->free;
        } else {
            ASSERT(target_fragment == head);
        }

        if (target_n) {
            nfree = target_n->free;
        } else {
            ASSERT(target_fragment == tail);
        }

        if (target_p) {
            if (!adjacency_check(target_p, target_fragment)) {
                Debug::log("Adjacency check for (prev, target) failed", DebugLevel::FATAL);
                ASSERT(false);
                return false;
            }
        }

        if (target_n) {
            if (!adjacency_check(target_fragment, target_n)) {
                Debug::log("Adjacency check for (target, next) failed", DebugLevel::FATAL);
                ASSERT(false);
                return false;
            }
        }

        if (!pfree && !nfree) {
            Debug::log("No need to coalesce. Surrounding blocks alloc'd", DebugLevel::INFO);
            return true;
        }

        if (pfree && target_p->prev && target_p->prev->free) {
            Debug::log("VIOLATED INVARIANT [DOUBLE FREE (PREV)]", DebugLevel::FATAL);
            ASSERT(pfree && !target_p->prev->free);
            return false;
        }

        if (nfree && target_n->next && target_n->next->free) {
            Debug::log("VIOLATED INVARIANT [DOUBLE FREE (NEXT)]", DebugLevel::FATAL);
            ASSERT(nfree && !target_n->next->free);
            return false;
        }

        
        // Since we had to check for existence before assigning the p/nfree - we know
        // by this point that p/nfree implies a non-null adjacent fragment
        if (pfree) {
             target_fragment->size = target_fragment->size + target_p->size;
             target_fragment->start = target_p-> start;

             MemoryFragment* p2 = target_p->prev;
             if (p2) {
                target_fragment->prev = p2;
                p2->next = target_fragment;
             } else {
                ASSERT(target_p == head);
                target_fragment->prev = nullptr;
                head = target_fragment;
             }

             delete target_p;
             target_p = nullptr;
        }

        if (nfree) {
            target_fragment->size = target_fragment->size + target_n->size;
            
            MemoryFragment* n2 = target_n->next;
            if (n2) {
                target_fragment->next = n2;
                n2->prev = target_fragment;
            } else {
                ASSERT(target_n == tail);
                target_fragment->next = nullptr;
                tail = target_fragment;
            }

            delete target_n;
            target_n = nullptr;
        }

        return true;
    }

};

enum alloc_tags {
    INIT, ACK, ALLOC, FREE, SHRINK, GROW, MISMATCH
};

struct FragmentInterval{
    unsigned int start;
    unsigned int size;
    alloc_tags status = INIT;

};

struct EntityMemory{
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

    void reset_flags() {ingested_flags = {};}
};

class MemoryManager {
public:
    MemoryManager(unsigned int size) :
        memory_list{MemoryList{size}},
        total_size{size},
        alloc_stats{0, 0, 0.0f, 0.0f},
        free_stats{1, size, (float)size, 0.0f},
        gpu_memory_heap(size)  {
        };
    ~MemoryManager() = default;

    unsigned int total_size;
    MemoryList memory_list;
    std::vector<RichVertex> gpu_memory_heap;

    struct MemoryStats {
        unsigned int n;
        unsigned int size;

        float mu;
        float M2;

        void add(unsigned int d_size) {
            unsigned int n_n = n + 1;
            unsigned int n_size = size + d_size;

            float d1 = (float)d_size - mu;
            
            float n_mu = mu + d1/n_n;

            float d2 = (float)d_size - n_mu;

            M2 = M2 + d1*d2;
            mu = n_mu;
            n = n_n;
            size = n_size;
        }

        void remove(unsigned int d_size) {
            ASSERT(n >= 1);
            unsigned int n_n = n - 1;
            if (n_n == 0) {
                Debug::log("Removing last element. Resetting Stats.", DebugLevel::INFO);
                *this = {};
                return;
            }

            // Should not be able to remove more than total size.
            int n_size = size - d_size;
            ASSERT(n_size > 0);

            float d1 = (float)d_size - mu;
            
            float n_mu = mu - d1/n_n;

            float d2 = (float)d_size - n_mu;

            M2 = M2 - d1*d2;
            mu = n_mu;
            n = n_n;
            size = (unsigned int)n_size;
        }

        float get_variance() {
            return (n>1) ? (M2/(n-1)) : 0.0f;
        }

        float get_entropy() {
            if (n == 0) return 0.0f;
            float var = get_variance();
            float cv_sq = (mu > 0.0f) ? var / (mu * mu) : 0.0f;
            return std::log(n) - 0.5*cv_sq;
        }

        bool is_empty() {
            return (n == 0 || size == 0 || mu == 0.0f);
        }

    };

    MemoryStats alloc_stats;
    MemoryStats free_stats;

public:
    void write_vertex_to_heap_idx(unsigned int heap_index, const RichVertex& vertex) {
        gpu_memory_heap[heap_index] = vertex;
    }

private:

    float ENTROPY_THRESHOLD = 1.0;

    static FragmentInterval get_memory_interval(const MemoryList::MemoryFragment& fragment){
         return FragmentInterval{fragment.start, fragment.size, alloc_tags::INIT};
    }

    MemoryList::MemoryFragment* walk_to(unsigned int target) {
        for (auto it = memory_list.begin(); it != memory_list.end(); it++) {
            if (it->start == target) {
                return &*it;
            }
        }
        return nullptr;
    }

    MemoryList::MemoryFragment* start_from_walk_to(MemoryList::MemoryFragment* init, unsigned int target, bool forward = true) {
        if (forward) {
            for (auto it = memory_list.iterate_from(init); it != memory_list.end(); it++) {
                if (it->start == target) {
                    return &*it;
                }
            }
        } else {
            for (auto it = memory_list.iterate_from(init); it != memory_list.begin(); it--) {
                if (it->start == target) {
                    return &*it;
                }
            }
        }
        return nullptr;
    }

public:

    using MemoryFragment = MemoryList::MemoryFragment;

    void free_memory(EntityMemory& container, int size = -1) {
        
        bool free_all = false;
        int remainder = 0;
        if (size == -1){ 
            free_all = true; 
            remainder = container.total_size; 
        } else {
            free_all = false;
            ASSERT(size != 0);
            remainder = size;
        }

        MemoryFragment* to_free = memory_list.tail;
        for (auto it = container.intervals.end(); it != container.intervals.begin() && remainder != 0; it--){
            unsigned int start_to_find = it->start;
            container.last_start = start_to_find;
            to_free = start_from_walk_to(to_free, start_to_find, false);
            unsigned int chunk_size = to_free->size;

            ASSERT(it->status == alloc_tags::ACK);

            if (!free_all && remainder <= chunk_size) {
                ASSERT(managed_shrink(to_free, chunk_size - remainder));
                it->status = alloc_tags::SHRINK;
                container.total_size = container.total_size - remainder;
                remainder = 0;

            } else {
                ASSERT(remainder >= chunk_size);
                ASSERT(managed_free(to_free));
                it->status = alloc_tags::FREE;
                container.total_intervals--;
                container.total_size = container.total_size - chunk_size;
                remainder = remainder - chunk_size;
                ASSERT(remainder >= 0); //If this ever goes below zero - then something dreadful has happened.
            }
        }
    }

    void resize_memory(EntityMemory& container, unsigned int new_size) {
        if (new_size == 0) {
            Debug::log("[RESIZE ZERO] Cannot resize to zero. Try free instead.", DebugLevel::INFO);
            return;
        }
        
        unsigned int delta_size;

        if (new_size > container.total_size) {
            delta_size = new_size - container.total_size;
            allocate_memory(delta_size, container);
            
        } else if (new_size < container.total_size) {
            delta_size = container.total_size - new_size;
            free_memory(container, (int)delta_size);
        } else {
            Debug::log("[RESIZE SAME] Not a valid new size.", DebugLevel::INFO);
        }
        
    }

    void allocate_memory(unsigned int num, EntityMemory& container) {
    
        float d_entropy = evaluate_alloc_strategy(num);

        if (d_entropy > ENTROPY_THRESHOLD) {
            allocate_using_append(num, container);
        } else {
            allocate_using_walk(num, container);
        }
    }

    void allocate_using_append(unsigned int num, EntityMemory& container) {
        MemoryFragment* target = memory_list.tail;
        if (managed_alloc(target, num)) {
            // This should push back tail to no longer be the current 
            // target pointer.
            ASSERT(target != memory_list.tail);
            FragmentInterval nfrag = get_memory_interval(*target);
            nfrag.status = alloc_tags::ALLOC;
            container.intervals.push_back(nfrag);
            container.total_size = container.total_size + num;
            container.total_intervals++;
            container.last_start = target->start;
        }
    }

    void allocate_using_walk(unsigned int num, EntityMemory& container) {
        unsigned int start_idx;
        if (!container.intervals.empty()){
            start_idx = container.last_start;
        } else {
            start_idx = 0;
        }
        MemoryFragment* target = walk_to(start_idx);

        int remaining_size = num;
        unsigned int frag_size;
        
        // First, attempt to fill up the current index.
        if (target->next->free) {
            
            if (remaining_size >= target->size + target->next->size) {
                frag_size = target->size + target->next->size;
            } else {
                frag_size = remaining_size;
            }

            managed_grow(target, frag_size);
            ASSERT(container.intervals.back().start == target->start);
            container.intervals.back().size = frag_size;
            container.last_start = target->start;
            container.total_size = container.total_size + frag_size - target->size;
            container.intervals.back().status = alloc_tags::GROW;

            remaining_size = remaining_size - frag_size;
        }

        while (remaining_size != 0) {
            if (target->free) {
                frag_size = target->size;
                if (remaining_size >= frag_size) {
                    managed_alloc(target, frag_size);
                    container.intervals.push_back({target->start, frag_size, alloc_tags::ALLOC});
                    container.total_size = container.total_size + frag_size - target->size;
                    container.total_intervals++;
                    container.last_start = target->start;
                    remaining_size = remaining_size - frag_size;
                } else {
                    managed_alloc(target, remaining_size);
                    container.intervals.push_back({target->start, (unsigned)remaining_size, alloc_tags::ALLOC});
                    container.total_size = container.total_size + remaining_size - target->size;
                    container.total_intervals++;
                    container.last_start = target->start;
                    remaining_size = 0;
                }
                target = target->next;
            }
        }
    }

    float evaluate_alloc_strategy(int proposed_memory_delta) {
        ASSERT(!free_stats.is_empty());
        ASSERT(proposed_memory_delta > 0);
        
        MemoryStats walk_alloc_cpy = alloc_stats;
        const float mu = free_stats.mu;

        unsigned int est_n = 1;
        
        if (mu > 0.0f){
            est_n = (unsigned int)std::ceil((float)proposed_memory_delta / mu);
        }

        est_n = std::min(est_n, free_stats.n);
        est_n = std::min(est_n, (unsigned)proposed_memory_delta); // Cannot have more allocations than bytes requested
        est_n = std::max(est_n, 1u); // At least one allocation

        
        unsigned int trunc_mu = proposed_memory_delta / est_n;
        unsigned int remainder = proposed_memory_delta - (est_n - 1) * trunc_mu;

        for (int i = 0; i < est_n - 1; i++) {
            walk_alloc_cpy.add(trunc_mu);
        }

        walk_alloc_cpy.add(remainder);

        MemoryStats append_alloc_cpy = alloc_stats;

        append_alloc_cpy.add(proposed_memory_delta);
        float entropy_delta = walk_alloc_cpy.get_entropy() - append_alloc_cpy.get_entropy();

        return entropy_delta;
    }

    bool managed_alloc(MemoryFragment* target, unsigned int size) {
        ASSERT(target->free);
        ASSERT(target->size >= size);
        unsigned int remainder = target->size - size;

        if (memory_list.allocate_into(target, size)) {
            ASSERT(!(target->free));

            // New alloc block + replace free block with a smaller one.
            alloc_stats.add(target->size);
            free_stats.remove(size);
            free_stats.add(remainder);
            return true;
        }

        return false;
    }

    bool managed_grow(MemoryFragment* target, unsigned int new_size) {
        ASSERT(!(target->free));

        unsigned int old_alloc_size = target->size;
        unsigned int old_free_tail_size = target->next->size;

        ASSERT((old_alloc_size + old_free_tail_size) <= new_size);

        if (memory_list.grow_allocated_block(target, new_size)) {
            alloc_stats.remove(old_alloc_size);
            free_stats.remove(old_free_tail_size);
            alloc_stats.add(new_size);
            free_stats.add(old_alloc_size + old_free_tail_size - new_size);
            return true;
        }
        return false;
    }

    bool managed_shrink(MemoryFragment* target, unsigned int new_size) {
        ASSERT(!(target->free));

        unsigned int old_alloc_size = target->size;
        unsigned int old_free_tail_size = target->next->size;

        ASSERT(old_alloc_size >= new_size);

        if (memory_list.shrink_allocated_block(target, new_size)) {
            alloc_stats.remove(old_alloc_size);
            free_stats.remove(old_free_tail_size);
            alloc_stats.add(new_size);
            free_stats.add(old_alloc_size + old_free_tail_size - new_size);
            return true;
        }
        return false;
    }
    bool managed_free(MemoryFragment* target) {
        ASSERT(!(target->free));
        if (memory_list.free_block(target)){
            ASSERT(target->free);
            alloc_stats.remove(target->size);
            free_stats.add(target->size);
            return true;
        }
        return false;
    }
};


class Renderer {
    public:
    Renderer() : 
    memory_manager{1024*128} {};
    ~Renderer() {
        cleanup();
    };

    public:

    unsigned int m_VAO_ID;
    unsigned int m_VBO_ID;
    unsigned int m_IBO_ID;

    // The IBO Register contains a mapping from UUID to EntityMemory.
    // EntityMemory's intervals contain start/size of the allocated fragments.
    // The mapping from local vertex indices to heap indices can be done
    // via a stepwise linear interpolation since we know the start/size of each fragment.
    std::unordered_map<uint64_t, EntityMemory> m_entity_registry;
    
    std::vector<unsigned int> m_IBO_mirror;
    bool m_IBO_dirty = false;

    // This is a pointer to one giant Vertex Buffer. We treat it as a heap for
    // since that's effectively what it is. Each "new entity" in the "batch render"
    // is essentially a "malloc" from this heap. Deleting is just "free".
    MemoryManager memory_manager;

    public:

    void init() {
        set_up_gpu_buffers();
    }

    void set_up_gpu_buffers() {
        m_entity_registry.reserve(1024); // Pre-allocate space for 1024 entities.

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        glGenVertexArrays(1, &m_VAO_ID);
        glGenBuffers(1, &m_VBO_ID);
        glGenBuffers(1, &m_IBO_ID);
        glBindVertexArray(m_VAO_ID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO_ID);
        glBufferData(GL_ARRAY_BUFFER, memory_manager.gpu_memory_heap.size() * sizeof(RichVertex), memory_manager.gpu_memory_heap.data(), GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(RichVertexAttr::BOUNDINGBOX_LOC, RichVertexAttr::BOUNDINGBOX_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::BOUNDINGBOX_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::BOUNDINGBOX_LOC);
        glVertexAttribPointer(RichVertexAttr::STARTPT_LOC, RichVertexAttr::STARTPT_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::STARTPT_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::STARTPT_LOC);
        glVertexAttribPointer(RichVertexAttr::ENDPT_LOC, RichVertexAttr::ENDPT_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::ENDPT_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::ENDPT_LOC);
        glVertexAttribPointer(RichVertexAttr::CONTROLPT1_LOC, RichVertexAttr::CONTROLPT1_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::CONTROLPT1_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::CONTROLPT1_LOC);
        glVertexAttribPointer(RichVertexAttr::CONTROLPT2_LOC, RichVertexAttr::CONTROLPT2_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::CONTROLPT2_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::CONTROLPT2_LOC);
        glVertexAttribPointer(RichVertexAttr::TEXCOORDS_LOC, RichVertexAttr::TEXCOORDS_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::TEXCOORDS_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::TEXCOORDS_LOC);
        // glVertexAttribIPointer(RichVertexAttr::THICKNESS_LOC, RichVertexAttr::THICKNESS_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::THICKNESS_OFFSET);
        // glEnableVertexAttribArray(RichVertexAttr::THICKNESS_LOC);
        // glVertexAttribIPointer(RichVertexAttr::FILLED_LOC, RichVertexAttr::FILLED_SIZE, GL_FLOAT, GL_FALSE, RichVertexAttr::STRIDE, (void*)RichVertexAttr::FILLED_OFFSET);
        // glEnableVertexAttribArray(RichVertexAttr::FILLED_LOC);

        glVertexAttribIPointer(RichVertexAttr::THICKNESS_LOC, 1, GL_INT, RichVertexAttr::STRIDE, (void*)RichVertexAttr::THICKNESS_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::THICKNESS_LOC);
        
        glVertexAttribIPointer(RichVertexAttr::FILLED_LOC, 1, GL_INT, RichVertexAttr::STRIDE, (void*)RichVertexAttr::FILLED_OFFSET);
        glEnableVertexAttribArray(RichVertexAttr::FILLED_LOC);
    }


    
    void cleanup() {
        glDeleteBuffers(1, &m_VBO_ID);
        glDeleteBuffers(1, &m_IBO_ID);
        glDeleteVertexArrays(1, &m_VAO_ID);
    }

    void start_frame() {

        float window_width = Orchestrator::get()->get_imgui_state().canvas_width;
        float window_height = Orchestrator::get()->get_imgui_state().canvas_height;
        float window_x = Orchestrator::get()->get_imgui_state().canvas_x_pos;  
        float window_y = Orchestrator::get()->get_imgui_state().canvas_y_pos;


        glViewport((GLint)window_x, (GLint)window_y, (GLsizei)window_width, (GLsizei)window_height);        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        process_request_buffer();

    }

    void render_frame() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO_ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IBO_mirror.size()*sizeof(unsigned int), m_IBO_mirror.data(), GL_DYNAMIC_DRAW);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_IBO_mirror.size(), GL_UNSIGNED_INT, 0);
    }

    void end_frame() {
        for (auto& [uuid, ent_mem] : m_entity_registry) {
            ent_mem.reset_flags();
        }
        ImGuiIO& io = ImGui::GetIO();
        glViewport(0,0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    }

    //struct EntityMemory{
    //    std::vector<FragmentInterval> intervals;
    //    unsigned int total_size = 0;
    //    unsigned int first_start = 0;
    //    unsigned int last_start = 0;
    //    unsigned int total_intervals = 0;
    //    struct ingested_flags {
    //        bool to_delete = false;
    //        std::optional<bool> visible = std::nullopt;
    //        bool size_changed = false;
    //        bool verts_changed = false;
    //        int first_seen_idx = -1;
    //    } ingested_flags;

    //    void reset_flags() {ingested_flags = {};}
    //};

    void process_request_buffer() {
        std::vector<EntityData*> ordered_entity_list;
        std::vector<EntityData*> to_delete_list;

        auto checkout = [&](EntityData* entity) -> EntityMemory& {
            EntityMemory& ent_mem = m_entity_registry.at(entity->UUID);
            if (ent_mem.ingested_flags.first_seen_idx < 0) {
                ordered_entity_list.push_back(entity);
                ent_mem.ingested_flags.first_seen_idx = ordered_entity_list.size();
            }
            return ent_mem;
        };

        while(!Entity::RenderRequestBuffer.empty()) {
            BufferRequest req = Entity::RenderRequestBuffer.front();
            Entity::RenderRequestBuffer.pop();
            
            EntityMemory& ent_mem = checkout(req.p_entity_data);


            if (req.type == BufferRequestType::REGISTER) {
                ent_mem.ingested_flags.size_changed = true;
                ent_mem.ingested_flags.verts_changed = true;
                ent_mem.ingested_flags.new_entity = true;
                m_IBO_dirty = true;
            } else if (req.type == BufferRequestType::SHOW) {
                ent_mem.ingested_flags.next_vis_state = true;
                m_IBO_dirty = true;
            } else if (req.type == BufferRequestType::HIDE) {
                ent_mem.ingested_flags.next_vis_state = false;
                m_IBO_dirty = true;
            } else if (req.type == BufferRequestType::UPDATE) {
                if (req.p_entity_data->vertex_count != ent_mem.total_size) {
                    ent_mem.ingested_flags.size_changed = true;
                    m_IBO_dirty = true;
                }
                ent_mem.ingested_flags.verts_changed = true;
            } else if (req.type == BufferRequestType::DELETE) {
                ent_mem.ingested_flags.to_delete = true;
                to_delete_list.push_back(req.p_entity_data);
                m_IBO_dirty = true;
            } else {
                Debug::log("Invalid Render Request.", DebugLevel::ERROR);
                ASSERT(false);
            }
        }

        for (auto p_entity_data : ordered_entity_list) {
            EntityMemory& ent_mem = m_entity_registry.at(p_entity_data->UUID);
            if(ent_mem.ingested_flags.to_delete) {
                // We free after all other mutations and IBO rewrites are complete.
                // This also lets us ignore everything.
                continue;
            }

            // Perform all data mutations here.
            if(ent_mem.ingested_flags.size_changed || ent_mem.ingested_flags.verts_changed) {
                if(ent_mem.ingested_flags.new_entity) {
                    register_new_entity(*p_entity_data);
                } else {
                    update_entity(*p_entity_data);
                }
            }

            // Perform all visibility toggles here.
            if (ent_mem.ingested_flags.next_vis_state != std::nullopt){
                ent_mem.visible = (bool)ent_mem.ingested_flags.next_vis_state;
            }
        }

        if (m_IBO_dirty) { // Skip if no size mutations happened
            int ibo_size = 0;
            m_IBO_mirror.clear(); // First, delete the current IBO mirror.
             // Loop through all entities and only calculate IBO if visible flag.
            for (auto& [uuid, ent_mem] : m_entity_registry) {
                if (ent_mem.visible){
                    ibo_size = ibo_size + ent_mem.cached_ibo_segment.size();
                }
            }
    
            m_IBO_mirror.resize(ibo_size);
            for (auto& [uuid, ent_mem] : m_entity_registry) {
                if (ent_mem.visible) {
                    m_IBO_mirror.insert(m_IBO_mirror.end(), ent_mem.cached_ibo_segment.begin(), ent_mem.cached_ibo_segment.end());
                }
            }
        }

        for (auto ent_dat : to_delete_list) {
            delete_entity(*ent_dat);
        }
    }

    int acknowledge_memory_allocation(const EntityData& entity) {
        if (m_entity_registry.find(entity.UUID) == m_entity_registry.end()) {
            Debug::log("Entity not registered with Renderer!");
            return 0;
        }

        int total_mutations = 0;
        
        EntityMemory& mem = m_entity_registry.at(entity.UUID);

        for (auto& fragment: mem.intervals) {
            if (fragment.status != alloc_tags::ACK) {
                total_mutations++;
            }
            fragment.status = alloc_tags::ACK;
        }
        return total_mutations;
    }

    int get_offset_index(int local_vertex_idx, std::vector<FragmentInterval> intervals){
        int to_rtn = local_vertex_idx;
        for (auto frag : intervals) {
            if (to_rtn < frag.size) {
                ASSERT(to_rtn >= 0);
                return to_rtn + frag.start;
            } else {
                to_rtn = to_rtn - frag.size;
            }
        }
    }

    void register_new_entity(const EntityData& entity) {

        ASSERT(entity.vertex_count > 0); // Entity should have at least one vertex. No placeholder handling for now.
        
        if (m_entity_registry.find(entity.UUID) != m_entity_registry.end()) {
            // Already registered
            Debug::log("Entity already registered with Renderer!", DebugLevel::WARN);
            return;
        }

        EntityMemory to_allocate;

        auto [it, inserted] = m_entity_registry.try_emplace(entity.UUID, to_allocate);
        if (inserted) {
            update_entity(entity);
        } else {
            Debug::log("Entity already registered with Renderer, despite passing original check!", DebugLevel::ERROR);
            return;
        }
    }
    void update_entity(const EntityData& entity) {

        if (m_entity_registry.find(entity.UUID) == m_entity_registry.end()) {
            Debug::log("Entity not registered with Renderer!");
            return;
        }
        
        EntityMemory& mem = m_entity_registry.at(entity.UUID);
        unsigned int entity_vertex_count = entity.vertex_count;

        unsigned int local_vertex = 0;
        unsigned int fragment_start = mem.first_start;

        if (entity.vertex_count != mem.total_size) {
            if (mem.intervals[0].status == alloc_tags::INIT) {
                // First time allocation
                memory_manager.allocate_memory(entity_vertex_count, mem);
                if (!acknowledge_memory_allocation(entity)) {
                    Debug::log("No allocs detected during init!", DebugLevel::ERROR);
                }
            } else {
                // Resize existing allocation
                memory_manager.resize_memory(mem, entity_vertex_count);
                if (!acknowledge_memory_allocation(entity)) {
                    Debug::log("No allocs detected during update!", DebugLevel::ERROR);
                }
            }

            // Now, update the cached IBO Segment using vertex draw order from the EntityData
            mem.cached_ibo_segment = {};
            for (int i = 0; i < entity.vertex_draw_orders.size(); i++){
                mem.cached_ibo_segment.push_back(get_offset_index(entity.vertex_draw_orders[i], mem.intervals));
            }

            ASSERT((mem.cached_ibo_segment.size() % 3 == 0));
        }

        for (auto& fragment: mem.intervals) {
            fragment_start = fragment.start;
            for (int i = 0; i < fragment.size; i++){ 
                memory_manager.write_vertex_to_heap_idx(fragment_start + i, entity.vertex_data[local_vertex]);
                local_vertex++;
            }
            glBufferSubData(GL_ARRAY_BUFFER, fragment_start*sizeof(RichVertex), 
                            fragment.size*sizeof(RichVertex), (void*)&memory_manager.gpu_memory_heap[fragment_start]);
        }
    }

    void delete_entity(const EntityData& entity) {

        if (m_entity_registry.find(entity.UUID) == m_entity_registry.end()) {
            Debug::log("Entity not registered with Renderer!");
            return;
        }
        memory_manager.free_memory(m_entity_registry[entity.UUID]);
        m_entity_registry.erase(entity.UUID);
    }
};

