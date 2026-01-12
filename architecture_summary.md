# Nobius Architecture Summary

## Overview

Nobius is a **decentralized, session-based application framework** built on three core principles:

1. **Intelligence lives in sessions, not in data** - Entities and assemblies are dumb primitives
2. **Priority-based resource claiming via history** - Sessions coordinate through per-frame history tracking
3. **1-frame latency is acceptable** - Eventual consistency enables composable, conflict-free updates

---

## 1. Primitives: The Atomic Global State

Primitives are the **only** global, persistent state in the system. They store data but have minimal logic.

### **EntityManager**
```cpp
// Stores all primitive entities (circles, rectangles, etc.)
class EntityManager {
    std::unordered_map<UUID, Entity> entities;
    
    // Entity = collection of components (Position, Velocity, Color, Vertices, etc.)
    template<typename T> T& get_component(UUID id);
    template<typename T> void update_component(UUID id, T component);
    
    // Query entities by component signature
    auto query<Position, Velocity>() -> std::vector<EntityRef>;
};
```

**Key properties:**
- Entities are **dumb data** (position, color, vertices)
- No semantic meaning (doesn't know what "selection" or "physics" means)
- Components are pure state (no behavior)

---

### **AssemblyManager**
```cpp
// Stores multi-entity structures (e.g., a person = 6 entities)
class AssemblyManager {
    struct Assembly {
        UUID id;
        std::vector<UUID> entity_ids;  // References to entities
        std::unordered_map<std::string, std::any> metadata;  // Custom data
    };
    
    std::unordered_map<UUID, Assembly> assemblies;
    
    Assembly& get_assembly(UUID id);
    std::vector<UUID> get_entities(UUID assembly_id);
};
```

**Key properties:**
- Assemblies are **flat** (no nested assemblies - max 1 level)
- Store metadata (e.g., `"physics_mode": "freerunning"`, `"animation_state": "paused"`)
- Acts as "recipes" for factory patterns (can rebuild from UUID + metadata)

---

### **InputSystem**
```cpp
// Accumulates timestamped input deltas
class InputSystem {
    struct TimeStampedValue<T> {
        T value;
        T delta;  // Change since last frame
        float timestamp;
    };
    
    TimeStampedValue<glm::vec2> mouse_position;
    TimeStampedValue<bool> mouse_buttons[3];
    TimeStampedValue<glm::vec2> scroll_delta;
    
    // Query interface
    glm::vec2 get_mouse_delta() const;
    bool was_mouse_button_just_pressed(int button) const;
    
    // Reset deltas at end of frame
    void clear_deltas();
};
```

**Key properties:**
- **History accumulator** for input state
- Stores deltas (what changed this frame)
- Provides gesture-like queries (`was_key_just_pressed`, `get_mouse_delta`)

---

### **Renderer**
```cpp
// Command-based rendering primitive
class Renderer {
    void submit_draw_call(/* vertices, shader, etc. */);
    void set_viewport(int x, int y, int w, int h);
    void present();
};
```

**Key properties:**
- Accepts render commands from sessions
- No knowledge of entities or game logic
- Just draws what it's told

---

### **CameraState**
```cpp
// Camera/viewport state primitive
class CameraState {
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;
    float zoom;
    glm::vec2 pan_offset;
};
```

**Key properties:**
- Pure state, no update logic
- Sessions read/write camera state as needed

---

### **Orchestrator**
```cpp
// The runtime that manages the session stack
class Orchestrator {
    std::vector<Session*> context_stack;
    
    void push_context(Session* ctx);
    void pop_context();
    
    void run_frame(float dt);  // Three-phase dispatch
};
```

**Key properties:**
- Owns the session stack
- Dispatches updates to sessions in priority order (top-down)
- Does NOT contain application logic

---

## 2. Engines: Stateless Operations

Engines are **pure functions** that perform computations. They hold **no state**.

```cpp
// Example: Transform engine (coordinate conversions)
class TransformEngine {
    glm::vec2 world_to_screen(glm::vec2 world_pos, const glm::mat4& view_matrix) const {
        // Pure function: input → output, no side effects
        return /* transformed position */;
    }
    
    glm::vec2 screen_to_world(glm::vec2 screen_pos, const glm::mat4& view_matrix) const {
        return /* transformed position */;
    }
};

// Example: Physics engine (force calculations)
class PhysicsEngine {
    glm::vec2 calculate_forces(Position pos, Velocity vel, Mass mass, float dt) const {
        // Stateless: compute forces based on inputs
        return gravity + drag + /* other forces */;
    }
};

// Example: Geometry engine (spatial queries)
class GeometryEngine {
    bool point_in_circle(glm::vec2 point, glm::vec2 center, float radius) const {
        return glm::distance(point, center) < radius;
    }
};
```

**Key properties:**
- **No state** (or minimal state like cached lookup tables)
- **Reusable** across sessions
- **Composable** (sessions can use multiple engines)
- Examples: TransformEngine, PhysicsEngine, GeometryEngine, SpringEngine, CoordinationEngine

---

## 3. Sessions: Behavioral Intelligence

Sessions are **smart** components that interpret primitives and use engines to implement behavior.

```cpp
class Session {
public:
    virtual ~Session() = default;
    
    // Lifecycle
    virtual void on_enter() {}  // Called when pushed onto stack
    virtual void on_exit() {}   // Called when popped from stack
    
    // Three-phase update
    virtual void process_input_deltas(InputSystem& input, DispatchHistory& history) {}
    virtual void on_update(float dt, DispatchHistory& history) {}
    virtual void on_render(DispatchHistory& history) {}
    
protected:
    // Sessions access primitives via references
    EntityManager& entity_mgr;
    AssemblyManager& assembly_mgr;
    Renderer& renderer;
    InputSystem& input_system;
    CameraState& camera_state;
    Orchestrator& orchestrator;
};
```

**Key properties:**
- **Own local state** (e.g., `SelectContext` owns `std::unordered_set<UUID> selected_entities`)
- **Hold UUIDs**, not objects (reference primitives by ID)
- **Compose engines** for computation
- **Can push/pop other sessions** (via `PushContext`/`PopContext` capabilities)
- **Short-lived** (pushed for specific interactions, popped when done)

---

### Example: SelectContext

```cpp
class SelectContext : public Session {
    std::unordered_set<UUID> selected_entities;  // Local state
    GeometryEngine geometry_engine;              // Composed engine
    
    void process_input_deltas(InputSystem& input, DispatchHistory& history) override {
        if (history.input_authority_claimed) return;  // Someone above us claimed input
        
        if (input.was_mouse_button_just_pressed(SDL_BUTTON_LEFT)) {
            // Use engine for computation
            auto mouse_pos = input.get_mouse_position();
            auto world_pos = transform_engine.screen_to_world(mouse_pos, camera_state.view);
            
            // Query primitive (EntityManager)
            for (auto [id, pos, radius] : entity_mgr.query<Position, Radius>()) {
                if (geometry_engine.point_in_circle(world_pos, pos, radius)) {
                    selected_entities.insert(id);  // Update local state
                    history.input_authority_claimed = true;  // Claim input
                    
                    // Push child session if already selected (start drag)
                    if (selected_entities.count(id)) {
                        orchestrator.push_context(new DragContext(id, selected_entities));
                    }
                    break;
                }
            }
        }
    }
    
    void on_render(DispatchHistory& history) override {
        // Render selection highlights
        for (auto id : selected_entities) {
            if (!history.entities_being_written.count(id)) {  // Skip if being dragged
                renderer.draw_highlight(entity_mgr.get_component<Position>(id));
            }
        }
    }
};
```

---

## 4. Sessions + Primitives + Engines → Complex Interactions

### **The Interaction Flow:**

1. **User interacts** → InputSystem accumulates deltas
2. **Orchestrator dispatches** → Sessions process deltas top-down
3. **Sessions query primitives** → Read entity/assembly state via UUIDs
4. **Sessions use engines** → Perform stateless computations
5. **Sessions update primitives** → Write back to EntityManager/AssemblyManager
6. **Sessions claim resources** → Mark entities in DispatchHistory
7. **Lower sessions respect claims** → Skip entities already handled by higher sessions

### **Example: Select → Drag → Rotate Workflow**

```
Frame 0:
Stack: [SelectContext]
- SelectContext owns selected_entities = {circle_A}

Frame 10: User clicks circle_A (already selected)
Stack: [SelectContext, DragContext]
- SelectContext detects click on selected entity
- SelectContext pushes DragContext(circle_A, selected_entities)

Frame 11-50: User drags
- DragContext.process_input_deltas():
  - Claims circle_A in history: entities_being_written.insert(circle_A)
  - Reads mouse delta from InputSystem
  - Updates entity position in EntityManager
- SelectContext.on_render():
  - Sees circle_A in history.entities_being_written
  - Skips rendering highlight (DragContext handles it)

Frame 30: User scrolls while dragging (rotate gesture)
Stack: [SelectContext, DragContext, RotateContext]
- DragContext detects scroll + drag combo
- DragContext pushes RotateContext(circle_A)

Frame 31-60: User rotates
- RotateContext claims circle_A, applies rotation
- DragContext sees circle_A claimed, skips movement
- SelectContext still just renders selection (no conflict)

Frame 61: User releases mouse
- RotateContext pops itself
- DragContext pops itself
Stack: [SelectContext]
- SelectContext resumes rendering highlight
- circle_A remains selected
```

---

## 5. Priority Stack + History: Decentralized Coordination

### **Why Priority Stack?**

Sessions are ordered by **priority** (top = highest):
- Higher sessions run first
- Higher sessions can block lower sessions via history

```
Stack (top to bottom):
┌──────────────────┐
│  DragContext     │  Priority 3 (runs first)
├──────────────────┤
│  SelectContext   │  Priority 2
├──────────────────┤
│  PhysicsContext  │  Priority 1 (runs last)
└──────────────────┘
```

---

### **Why History, Not "Per-Event" Ingestion?**

**Per-Event Model (BAD):**
```cpp
// Each event propagates down stack until consumed
dispatch_event(MouseClick event) {
    for (ctx : context_stack) {
        ctx->on_event(event);
        if (event.consumed) break;  // Stop propagation
    }
}
```

**Problems:**
- ❌ Can only consume entire event (all-or-nothing)
- ❌ Can't express partial resource claims (entity A vs entity B)
- ❌ Tight temporal coupling (must decide immediately)
- ❌ No intra-frame coordination between sessions

---

**History Model (GOOD):**
```cpp
struct DispatchHistory {
    // Granular resource tracking
    std::unordered_set<UUID> entities_being_written;
    std::unordered_set<UUID> entities_being_read_exclusive;
    bool input_authority_claimed = false;
    bool camera_locked = false;
    
    // Shared data (for cross-session communication)
    std::unordered_map<std::string, std::any> shared_context_data;
};

void dispatch_input_deltas() {
    DispatchHistory history;  // Fresh history per frame
    
    for (auto ctx : context_stack) {
        ctx->process_input_deltas(input_system, history);
        // Each session reads history, updates history, continues
    }
}
```

**Benefits:**
- ✅ **Granular claims** (entity A vs B, not "all input")
- ✅ **Composable** (multiple sessions can coexist peacefully)
- ✅ **Decoupled** (sessions don't know about each other)
- ✅ **Intra-frame coordination** (via `shared_context_data`)
- ✅ **Priority-based preemption** (first to claim wins)

---

### **History Example: Physics vs Drag**

```cpp
// Frame N:

// DragContext (top) runs first:
void DragContext::process_input_deltas(InputSystem& input, DispatchHistory& history) {
    // Claim entities we're dragging
    for (auto id : dragging_entities) {
        history.entities_being_written.insert(id);
    }
    
    // Move entities
    auto delta = input.get_mouse_delta();
    for (auto id : dragging_entities) {
        auto pos = entity_mgr.get_component<Position>(id);
        entity_mgr.update_component<Position>(id, pos + delta);
    }
}

// PhysicsContext (bottom) runs second:
void PhysicsContext::on_update(float dt, DispatchHistory& history) {
    for (auto [id, pos, vel] : entity_mgr.query<Position, Velocity>()) {
        // Check history: is this entity being manipulated?
        if (history.entities_being_written.count(id)) {
            continue;  // Skip physics for dragged entities
        }
        
        // Apply physics to unclaimed entities
        auto force = physics_engine.calculate_forces(pos, vel);
        entity_mgr.update_component<Velocity>(id, vel + force * dt);
        
        // Mark that we wrote to this entity
        history.entities_being_written.insert(id);
    }
}

// Result: Dragged entities skip physics, free entities get physics
// No conflicts, no coordination needed!
```

---

## 6. Complex Use Case: Dynamic Physics + Baked Animation System

Let's design a system where entities can **dynamically switch** between:
1. **Physics simulation** (gravity, collisions, forces)
2. **Baked animation** (keyframe playback along paths)
3. **User manipulation** (drag, rotate via UI)

And all three can **coexist** for different entities simultaneously.

---

### **Setup: The Primitives**

```cpp
// EntityManager: Stores all entities
// Each entity has: Position, Velocity, Color, Vertices

// AssemblyManager: Stores animation data
struct Assembly {
    UUID id;
    std::vector<UUID> entity_ids;
    
    // Metadata for animation/physics state
    std::unordered_map<std::string, std::any> metadata = {
        {"mode", "physics"},  // "physics", "animated", "user_controlled"
        {"animation_clip", "walk_cycle.anim"},
        {"animation_time", 0.0f},
        {"physics_paused", false}
    };
};

// AnimationClipManager: Stores baked keyframes
class AnimationClipManager {
    struct Keyframe {
        float time;
        std::unordered_map<UUID, glm::vec2> entity_positions;  // Entity positions at this time
    };
    
    struct AnimationClip {
        std::string name;
        std::vector<Keyframe> keyframes;
        float duration;
    };
    
    std::unordered_map<std::string, AnimationClip> clips;
    
    glm::vec2 sample(const std::string& clip_name, UUID entity_id, float time) const;
};
```

---

### **The Sessions**

#### **1. PhysicsContext (Always Active)**
```cpp
class PhysicsContext : public Session {
    PhysicsEngine physics_engine;
    
    void on_update(float dt, DispatchHistory& history) override {
        for (auto [id, pos, vel] : entity_mgr.query<Position, Velocity>()) {
            // Skip if:
            // - Entity claimed by higher session (drag, animation)
            // - Entity's assembly metadata says physics is paused
            if (history.entities_being_written.count(id)) continue;
            
            auto assembly_id = entity_mgr.get_assembly_for_entity(id);
            if (assembly_id) {
                auto& assembly = assembly_mgr.get_assembly(*assembly_id);
                if (assembly.metadata["mode"] != "physics") continue;
                if (std::any_cast<bool>(assembly.metadata["physics_paused"])) continue;
            }
            
            // Apply physics
            auto force = physics_engine.calculate_forces(pos, vel, dt);
            entity_mgr.update_component<Velocity>(id, vel + force * dt);
            entity_mgr.update_component<Position>(id, pos + vel * dt);
            
            history.entities_being_written.insert(id);
        }
    }
};
```

---

#### **2. AnimationContext (Always Active)**
```cpp
class AnimationContext : public Session {
    AnimationClipManager& clip_mgr;
    
    void on_update(float dt, DispatchHistory& history) override {
        // Iterate all assemblies with animation data
        for (auto& [assembly_id, assembly] : assembly_mgr.get_all_assemblies()) {
            if (assembly.metadata["mode"] != "animated") continue;
            
            // Advance animation time
            float& anim_time = std::any_cast<float&>(assembly.metadata["animation_time"]);
            anim_time += dt;
            
            auto clip_name = std::any_cast<std::string>(assembly.metadata["animation_clip"]);
            auto& clip = clip_mgr.get_clip(clip_name);
            
            // Loop animation
            if (anim_time > clip.duration) {
                anim_time = std::fmod(anim_time, clip.duration);
            }
            
            // Update all entities in assembly
            for (auto entity_id : assembly.entity_ids) {
                if (history.entities_being_written.count(entity_id)) continue;  // Skip if dragged
                
                // Sample animation clip at current time
                auto pos = clip_mgr.sample(clip_name, entity_id, anim_time);
                entity_mgr.update_component<Position>(entity_id, pos);
                
                history.entities_being_written.insert(entity_id);
            }
        }
    }
};
```

---

#### **3. SelectContext (User pushes/pops)**
```cpp
class SelectContext : public Session {
    std::unordered_set<UUID> selected_assemblies;
    
    void process_input_deltas(InputSystem& input, DispatchHistory& history) override {
        if (history.input_authority_claimed) return;
        
        if (input.was_mouse_button_just_pressed(SDL_BUTTON_LEFT)) {
            auto world_pos = get_mouse_world_pos();
            
            // Pick assembly under cursor
            for (auto& [assembly_id, assembly] : assembly_mgr.get_all_assemblies()) {
                if (is_assembly_at(assembly, world_pos)) {
                    selected_assemblies.clear();
                    selected_assemblies.insert(assembly_id);
                    history.input_authority_claimed = true;
                    
                    // If already selected, start drag
                    if (selected_assemblies.count(assembly_id)) {
                        orchestrator.push_context(new DragAssemblyContext(assembly_id, selected_assemblies));
                    }
                    break;
                }
            }
        }
    }
    
    void on_render(DispatchHistory& history) override {
        // Render selection boxes around selected assemblies
        for (auto assembly_id : selected_assemblies) {
            if (!history.assemblies_being_manipulated.count(assembly_id)) {
                draw_selection_box(assembly_id);
            }
        }
    }
};
```

---

#### **4. DragAssemblyContext (Pushed by SelectContext)**
```cpp
class DragAssemblyContext : public Session {
    UUID primary_assembly;
    std::unordered_set<UUID>& shared_selection;
    
    DragAssemblyContext(UUID primary, std::unordered_set<UUID>& sel)
        : primary_assembly(primary), shared_selection(sel) {}
    
    void process_input_deltas(InputSystem& input, DispatchHistory& history) override {
        history.input_authority_claimed = true;
        
        auto delta = input.get_mouse_delta();
        auto world_delta = transform_engine.screen_delta_to_world(delta);
        
        // Claim all entities in dragged assemblies
        for (auto assembly_id : shared_selection) {
            auto& assembly = assembly_mgr.get_assembly(assembly_id);
            
            // **Pause physics/animation while dragging**
            assembly.metadata["mode"] = "user_controlled";
            
            for (auto entity_id : assembly.entity_ids) {
                history.entities_being_written.insert(entity_id);
                
                // Move entity
                auto pos = entity_mgr.get_component<Position>(entity_id);
                entity_mgr.update_component<Position>(entity_id, pos + world_delta);
            }
        }
        
        // Release mouse → resume previous mode
        if (!input.is_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            for (auto assembly_id : shared_selection) {
                auto& assembly = assembly_mgr.get_assembly(assembly_id);
                // TODO: Remember previous mode, restore it
                assembly.metadata["mode"] = "physics";  // Default to physics
            }
            orchestrator.pop_context();  // Remove self
        }
    }
};
```

---

#### **5. ModeToggleContext (UI, Always Active)**
```cpp
class ModeToggleContext : public Session {
    void on_render(DispatchHistory& history) override {
        // ImGui toolbar
        if (ImGui::Button("Toggle Physics")) {
            for (auto assembly_id : get_selected_assemblies()) {
                auto& assembly = assembly_mgr.get_assembly(assembly_id);
                
                if (assembly.metadata["mode"] == "physics") {
                    assembly.metadata["mode"] = "animated";
                } else {
                    assembly.metadata["mode"] = "physics";
                }
            }
        }
        
        if (ImGui::Button("Play Animation")) {
            for (auto assembly_id : get_selected_assemblies()) {
                auto& assembly = assembly_mgr.get_assembly(assembly_id);
                assembly.metadata["mode"] = "animated";
                assembly.metadata["animation_time"] = 0.0f;
            }
        }
    }
};
```

---

### **The Interaction Scenario**

**Frame 0-100:** Three assemblies in scene:
- Assembly A: `mode = "physics"` → PhysicsContext updates (falling, gravity)
- Assembly B: `mode = "animated"` → AnimationContext updates (walking cycle)
- Assembly C: `mode = "physics"` → PhysicsContext updates (rolling on ground)

**Frame 101:** User clicks Assembly A (physics object)
- SelectContext detects click, selects Assembly A
- Stack: `[PhysicsContext, AnimationContext, ModeToggleContext, SelectContext]`

**Frame 102:** User clicks Assembly A again (already selected)
- SelectContext pushes DragAssemblyContext
- Stack: `[PhysicsContext, AnimationContext, ModeToggleContext, SelectContext, DragAssemblyContext]`

**Frame 103-200:** User drags Assembly A
- DragAssemblyContext claims all entities in Assembly A
- Sets `Assembly A.metadata["mode"] = "user_controlled"`
- PhysicsContext sees `mode != "physics"`, skips Assembly A
- AnimationContext sees `mode != "animated"`, skips Assembly A
- Assembly B still animating, Assembly C still physics

**Frame 201:** User releases mouse
- DragAssemblyContext restores `Assembly A.metadata["mode"] = "physics"`
- DragAssemblyContext pops itself
- Stack: `[PhysicsContext, AnimationContext, ModeToggleContext, SelectContext]`

**Frame 202-300:** Assembly A resumes physics
- PhysicsContext sees `mode == "physics"`, resumes updating Assembly A
- Assembly A continues falling from where it was dropped

**Frame 301:** User clicks "Toggle Physics" UI button while Assembly A selected
- ModeToggleContext changes `Assembly A.metadata["mode"] = "animated"`

**Frame 302-400:** Assembly A now animates
- AnimationContext sees `mode == "animated"`, starts playing animation
- PhysicsContext sees `mode != "physics"`, skips Assembly A

---

### **Key Takeaways**

1. **Three modes coexist**: Physics, animation, and user control all run simultaneously
2. **Metadata drives behavior**: Assembly metadata determines which session updates it
3. **History prevents conflicts**: Dragged entities blocked from physics/animation
4. **Sessions are composable**: SelectContext + DragContext + PhysicsContext + AnimationContext all stack
5. **No coupling**: PhysicsContext doesn't know about AnimationContext or DragContext
6. **UI can modify metadata**: ModeToggleContext changes assembly modes via UI
7. **Graceful transitions**: Drag pauses physics/animation, release resumes seamlessly

---

## Conclusion

The Nobius architecture achieves:
- ✅ **Decentralization** (no global managers, sessions own state)
- ✅ **Composability** (sessions stack freely, interact via history)
- ✅ **Flexibility** (add new sessions without modifying existing ones)
- ✅ **Simplicity** (primitives are dumb, engines are stateless, sessions are smart)
- ✅ **Performance** (1-frame latency is imperceptible, efficient resource claiming)

**Core Formula:**
```
Primitives (global state) + Engines (stateless operations) + Sessions (smart behavior) + History (coordination) = Composable, Conflict-Free, Dynamic System
```

**Philosophy:**
> "The stack is a dynamic system. Sessions update sections of global state based on their own state, inputs, and time. Worst case: 1-frame delay. Best case: seamless, emergent complexity from simple, independent components."
